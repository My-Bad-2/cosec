#include <kernel.h>
#include <stddef.h>
#include <stdint.h>

#include <debug/log.hpp>

#include <memory/memory.hpp>
#include <memory/physical.hpp>
#include <memory/virtual.hpp>

#include <system/cpu.hpp>

namespace kernel::memory::virt {
enum {
    Present = (1 << 0),
    Write = (1 << 1),
    User_super = (1 << 2),
    Write_through = (1 << 3),
    Cache_disable = (1 << 4),  // PCD
    Accessed = (1 << 5),
    Larger_pages = (1 << 7),
    PAT4k = (1 << 7),  // PAT lvl1
    Global = (1 << 8),
    Custom0 = (1 << 9),
    Custom1 = (1 << 10),
    Custom2 = (1 << 11),
    PAT_lg = (1 << 12),  // PAT lvl2+
    No_exec = (1UL << 63)
};

struct [[gnu::packed]] pagetable {
    pt_entry entries[512];
};

uintptr_t pa_mask = 0x000FFFFFFFFFF000;
static bool gib1_pages = false;

static pagetable* get_next_lvl(pagetable* curr_lvl, uint64_t entry,
                               bool allocate = true) {
    if (curr_lvl == nullptr) {
        return nullptr;
    }

    pagetable* ret = nullptr;

    if (curr_lvl->entries[entry].get_flags(Present)) {
        ret = reinterpret_cast<pagetable*>(
            to_higher_half(curr_lvl->entries[entry].get_addr()));
    } else if (allocate) {
        ret = new pagetable;
        curr_lvl->entries[entry].set_addr(
            from_higher_half(reinterpret_cast<uint64_t>(ret)));
        curr_lvl->entries[entry].set_flags(Present | Write | User_super, true);
    }

    return ret;
}

pt_entry* pagemap::virt_to_pte(uint64_t virt_addr, bool allocate,
                               uint64_t psize) {
    uintptr_t pml5_entry = (virt_addr & (0x1FFull << 48)) >> 48;
    uintptr_t pml4_entry = (virt_addr & (0x1FFull << 39)) >> 39;
    uintptr_t pml3_entry = (virt_addr & (0x1FFull << 30)) >> 30;
    uintptr_t pml2_entry = (virt_addr & (0x1FFull << 21)) >> 21;
    uintptr_t pml1_entry = (virt_addr & (0x1FFull << 12)) >> 12;

    pagetable *pml4, *pml3, *pml2, *pml1;

    pml4 = (la57_request.response != nullptr)
               ? get_next_lvl(this->toplvl, pml5_entry, allocate)
               : this->toplvl;
    if (pml4 == nullptr) {
        return nullptr;
    }

    pml3 = get_next_lvl(pml4, pml4_entry, allocate);
    if (pml3 == nullptr) {
        return nullptr;
    }

    if (psize == this->llpage_size) {
        return &pml3->entries[pml3_entry];
    }

    pml2 = get_next_lvl(pml3, pml3_entry, allocate);
    if (pml2 == nullptr) {
        return nullptr;
    }

    if (psize == this->lpage_size) {
        return &pml2->entries[pml2_entry];
    }

    pml1 = get_next_lvl(pml2, pml2_entry, allocate);
    if (pml1 == nullptr) {
        return nullptr;
    }

    return &pml1->entries[pml1_entry];
}

uintptr_t pagemap::virt_to_phys(uintptr_t virt_addr, size_t flags) {
    size_t psize = this->get_psize(flags);
    pt_entry* pml_entry = this->virt_to_pte(virt_addr, false, psize);

    if (pml_entry == nullptr || !pml_entry->get_flags(Present)) {
        return invalid_addr;
    }

    return pml_entry->get_addr() + (virt_addr % psize);
}

static uintptr_t cache_to_flags(Caching cache, bool large_pages) {
    uintptr_t patbit = (large_pages ? PAT_lg : PAT4k);
    uintptr_t ret = 0;

    switch (cache) {
        case UNCACHABLE:
            break;
        case WRITE_COMBINING:
            ret |= Write_through;
            break;
        case WRITE_THROUGH:
            ret |= patbit;
            break;
        case WRITE_PROTECTED:
            ret |= patbit | Write_through;
            break;
        case WRITE_BACK:
            ret |= patbit | Cache_disable;
            break;
        default:
            break;
    }

    return ret;
}

uintptr_t pagemap::map(uintptr_t virt_addr, uintptr_t phys_addr, size_t flags,
                       Caching cache) {
    auto map_one = [this](uintptr_t virt_addr, uintptr_t phys_addr,
                          size_t flags, Caching cache, size_t psize) {
        pt_entry* pml_entry = this->virt_to_pte(virt_addr, true, psize);

        if (pml_entry == nullptr) {
            Hex64 address = virt_addr;
            log::error << "Couldn't get page map entry for address: " << address
                       << '\n';

            return false;
        }

        auto real_flags = flags_to_arch(flags) |
                          cache_to_flags(cache, psize != this->page_size);

        pml_entry->set_addr(phys_addr);
        pml_entry->set_flags(real_flags, true);

        return true;
    };

    size_t psize = this->get_psize(flags);

    if (psize == this->llpage_size && !gib1_pages) {
        flags &= ~LLPAGE;
        flags |= LPAGE;

        for (size_t i = 0; i < GiB1; i += MiB2) {
            if (!map_one(virt_addr + i, phys_addr + i, flags, cache, MiB2)) {
                return false;
            }
        }

        return true;
    }
    return map_one(virt_addr, phys_addr, flags, cache, psize);
}

bool pagemap::unmap(uintptr_t virt_addr, size_t flags) {
    auto unmap_one = [this](uintptr_t virt_addr, size_t psize) {
        pt_entry* pml_entry = this->virt_to_pte(virt_addr, false, psize);

        if (pml_entry == nullptr) {
            Hex64 address = virt_addr;
            log::error << "Could not get page map entry for address " << address
                       << "\n";

            return false;
        }

        pml_entry->value = 0;
        system::cpu::invlpg(virt_addr);
        return true;
    };

    size_t psize = this->get_psize(flags);

    if (psize == this->llpage_size && !gib1_pages) {
        flags &= ~LLPAGE;
        flags |= LPAGE;

        for (size_t i = 0; i < GiB1; i += MiB2) {
            if (!unmap_one(virt_addr + i, MiB2)) {
                return false;
            }
        }

        return true;
    }

    return unmap_one(virt_addr, psize);
}

bool pagemap::set_flags(uintptr_t virt_addr, size_t flags, Caching cache) {
    size_t psize = this->get_psize(flags);
    pt_entry* pml_entry = this->virt_to_pte(virt_addr, true, psize);

    if (pml_entry == nullptr) {
        Hex64 address = virt_addr;
        log::error << "Could not get page map entry for address " << address
                   << "\n";

        return false;
    }

    size_t real_flags =
        flags_to_arch(flags) | cache_to_flags(cache, psize != this->page_size);
    uintptr_t addr = pml_entry->get_addr();

    pml_entry->value = 0;
    pml_entry->set_addr(addr);
    pml_entry->set_flags(real_flags, true);

    return true;
}

void pagemap::load() {
    system::cpu::write_cr3(
        from_higher_half(reinterpret_cast<uint64_t>(this->toplvl)));
}

void pagemap::save() {
    this->toplvl =
        reinterpret_cast<pagetable*>(to_higher_half(system::cpu::read_cr3()));
}

static size_t counter = 0;

pagemap::pagemap() : toplvl(new pagetable) {
    this->llpage_size = GiB1;
    this->lpage_size = MiB2;
    this->page_size = KiB4;

    if (kernel_pagemap == nullptr) {
        for (size_t i = 256; i < 512; i++) {
            get_next_lvl(this->toplvl, i, true);
        }

        if (counter < smp_request.response->cpu_count) {
            system::cpu::enable_pat();

            counter++;
        }
    } else {
        for (size_t i = 256; i < 512; i++) {
            this->toplvl->entries[i] = kernel_pagemap->toplvl->entries[i];
        }
    }
}

uintptr_t flags_to_arch(size_t flags) {
    uintptr_t ret = Present;

    if (flags & WRITE) {
        ret |= Write;
    }

    if (flags & USER) {
        ret |= User_super;
    }

    if (!(flags & EXEC)) {
        ret |= No_exec;
    }

    if (flags & GLOBAL) {
        ret |= Global;
    }

    if (is_lpage(flags)) {
        ret |= Larger_pages;
    }

    return ret;
}

bool is_canonical(uintptr_t addr) {
    if (la57_request.response != nullptr) {
        return (addr <= 0x00FFFFFFFFFFFFFF) || (addr >= 0xFF00000000000000);
    }

    return (addr <= 0x00007FFFFFFFFFFF) || (addr >= 0xFFFF800000000000);
}

static void destroy_level(pagetable* pml, size_t start, size_t end,
                          uint64_t level) {
    if (level == 0) {
        return;
    }

    for (size_t i = start; i < end; i++) {
        pagetable* next = get_next_lvl(pml, i, false);

        if (next == nullptr) {
            continue;
        }

        destroy_level(next, 0, 512, level - 1);
    }

    delete pml;
}

void destroy_pagemap(pagemap* map) {
    destroy_level(map->toplvl, 0, 256,
                  (la57_request.response != nullptr) ? 5 : 4);
}

void arch_init() {
    uint32_t a, b, c, d;
    gib1_pages = system::cpu::id(0x80000001, 0, a, b, c, d) &&
                 ((d & 1 << 26) == (1 << 26));
}

pagemap::~pagemap() {
    destroy_pagemap(this);
}
}  // namespace kernel::memory::virt