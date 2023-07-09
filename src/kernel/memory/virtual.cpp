#include <arch.hpp>
#include <stddef.h>
#include <stdint.h>

#include <debug/log.hpp>

#include <memory/memory.hpp>
#include <memory/physical.hpp>
#include <memory/virtual.hpp>

namespace kernel::memory::virt {
pagemap* kernel_pagemap = nullptr;

void init() {
    limine_memmap_entry** memmaps = memmap_request.response->entries;
    size_t memmap_count = memmap_request.response->entry_count;

    if (arch_init) {
        arch_init();
    }

    kernel_pagemap = new pagemap();

    auto [psize, flags] = kernel_pagemap->required_size(GiB1 * 4);

    for (size_t i = 0; i < GiB1 * 4; i += psize) {
        if (!(kernel_pagemap->map(i, i, RWX | flags))) {
            Hex64 address = i;
            log::error << "Can not map pagemap at address " << address << "\n";

            while (true) {
                disable_interrupts();
                halt();
            }
        }

        if (!(kernel_pagemap->map(to_higher_half(i), i, RW | flags))) {
            Hex64 address = to_higher_half(i);
            log::error << "Can not map pagemap at address " << address << "\n";

            while (true) {
                disable_interrupts();
                halt();
            }
        }
    }

    for (size_t i = 0; i < memmap_count; i++) {
        limine_memmap_entry* mmap = memmaps[i];

        uintptr_t base = align_down(mmap->base, kernel_pagemap->page_size);
        uintptr_t top =
            align_up(mmap->base + mmap->length, kernel_pagemap->page_size);

        if (top < GiB1 * 4) {
            continue;
        }

        Caching cache = default_caching;
        if (mmap->type == LIMINE_MEMMAP_FRAMEBUFFER) {
            cache = FRAMEBUFFER;
        }

        size_t size = top - base;
        auto [psize, flags] = kernel_pagemap->required_size(size);

        uint64_t al_size = align_down(size, psize);
        uint64_t diff = size - al_size;

        for (uint64_t t = base; t < (base + al_size); t += psize) {
            if (t < GiB1 * 4) {
                continue;
            }

            if (!(kernel_pagemap->map(t, t, RWX | flags, cache))) {
                Hex64 address = t;

                log::error << "Can not map pagemap at address " << address
                           << "\n";

                while (true) {
                    disable_interrupts();
                    halt();
                }
            }

            if (!(kernel_pagemap->map(to_higher_half(t), t, RWX | flags,
                                      cache))) {
                Hex64 address = to_higher_half(t);

                log::error << "Can not map pagemap at address " << address
                           << "\n";

                while (true) {
                    disable_interrupts();
                    halt();
                }
            }
        }

        base += al_size;

        for (uintptr_t t = base; t < (base + diff);
             t += kernel_pagemap->page_size) {
            if (t < GiB1 * 4) {
                continue;
            }

            if (!(kernel_pagemap->map(t, t, RWX, cache))) {
                Hex64 address = t;

                log::error << "Can not map pagemap at address " << address
                           << "\n";

                while (true) {
                    disable_interrupts();
                    halt();
                }
            }

            if (!(kernel_pagemap->map(to_higher_half(t), t, RWX, cache))) {
                Hex64 address = to_higher_half(t);

                log::error << "Can not map pagemap at address " << address
                           << "\n";

                while (true) {
                    disable_interrupts();
                    halt();
                }
            }
        }
    }

    for (size_t i = 0; i < kernel_file_request.response->kernel_file->size;
         i += kernel_pagemap->page_size) {
        uintptr_t phys_addr =
            kernel_address_request.response->physical_base + i;
        uintptr_t virt_addr = kernel_address_request.response->virtual_base + i;

        if (!(kernel_pagemap->map(virt_addr, phys_addr, RWX))) {
            Hex64 address = virt_addr;

            log::error << "Can not map pagemap at address " << address << "\n";

            while (true) {
                disable_interrupts();
                halt();
            }
        }
    }

    kernel_pagemap->load();

    log::info << "Initialized Virtual Memory Management!\n";
}
}  // namespace kernel::memory::virt