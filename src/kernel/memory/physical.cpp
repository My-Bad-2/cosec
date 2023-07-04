#include <debug/log.hpp>
#include <memory/bitmap.hpp>
#include <memory/memory.hpp>
#include <memory/physical.hpp>

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include <algorithm>
#include <mutex>
#include "arch.hpp"
#include <kernel.h>

namespace kernel::memory::physical {
static uintptr_t mem_usable_top = 0;
static size_t last_index = 0;
static Bitmap bitmap;
static std::mutex lock;

size_t usable_mem = 0;
size_t total_mem = 0;
size_t used_mem = 0;

uintptr_t mem_top = 0;

size_t total() {
    return total_mem;
}

size_t usable() {
    return usable_mem;
}

size_t used() {
    return used_mem;
}

size_t free() {
    return usable_mem - used_mem;
}

void* alloc_impl(size_t count, size_t limit) {
    size_t index = 0;
    // std::unique_lock guard(lock);

    while (last_index < limit) {
        if (!bitmap[last_index++]) {
            if (++index == count) {
                size_t page = last_index - count;

                for (size_t i = page; i < last_index; i++) {
                    bitmap.set(i, true);
                }

                return reinterpret_cast<void*>(page * PAGE_SIZE);
            }
        } else {
            index = 0;
        }
    }

    return nullptr;
}

void* alloc(size_t count) {
    std::unique_lock guard(lock);
    
    if (count == 0) {
        return nullptr;
    }

    size_t index = last_index;
    void* ret = alloc_impl(count, mem_usable_top / PAGE_SIZE);

    if (ret == nullptr) {
        last_index = 0;

        ret = alloc_impl(count, index);

        if (ret == nullptr) {
            log::warn << "Memory Manager: Out of Memory\n";

            while (true) {
                disable_interrupts();
                halt();
            }
        }
    }

    memset(to_higher_half(ret), 0, count * PAGE_SIZE);
    used_mem += count * PAGE_SIZE;

    return ret;
}

void free(void* ptr, size_t count) {
    if (ptr == nullptr) {
        return;
    }

    std::unique_lock guard(lock);
    uintptr_t page = reinterpret_cast<uintptr_t>(ptr) / PAGE_SIZE;

    for (size_t i = page; i < (page + count); i++) {
        bitmap.set(i, false);
    }

    last_index = std::min(last_index, page);
    used_mem -= count * PAGE_SIZE;
}

void init() {
    limine_memmap_entry** memmaps = memmap_request.response->entries;
    size_t memmap_count = memmap_request.response->entry_count;

    for (size_t i = 0; i < memmap_count; i++) {
        uintptr_t top = memmaps[i]->base + memmaps[i]->length;
        mem_top = std::max(mem_top, top);

        switch (memmaps[i]->type) {
            case LIMINE_MEMMAP_USABLE:
                usable_mem += memmaps[i]->length;
                break;
            case LIMINE_MEMMAP_KERNEL_AND_MODULES:
            case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
                used_mem += memmaps[i]->length;
                break;
            default:
                continue;
        }

        total_mem += memmaps[i]->length;
        mem_usable_top = std::max(mem_usable_top, top);
    }

    size_t bitmap_size = align_up((mem_usable_top / PAGE_SIZE) / 8, PAGE_SIZE);

    for (size_t i = 0; i < memmap_count; i++) {
        if (memmaps[i]->type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        if (memmaps[i]->length >= bitmap_size) {
            bitmap.buffer =
                reinterpret_cast<uint8_t*>(to_higher_half(memmaps[i]->base));
            bitmap.size = bitmap_size;

            memset(bitmap.buffer, 0xFF, bitmap.size);

            memmaps[i]->length -= bitmap_size;
            memmaps[i]->base += bitmap_size;

            used_mem += bitmap_size;
            break;
        }
    }

    for (size_t i = 0; i < memmap_count; i++) {
        if (memmaps[i]->type != LIMINE_MEMMAP_USABLE) {
            continue;
        }

        for (uintptr_t t = 0; t < memmaps[i]->length; t += PAGE_SIZE) {
            bitmap.set((memmaps[i]->base + t) / PAGE_SIZE, false);
        }
    }

    mem_top = align_up(mem_top, 0x40000000);

    log::info << "Initialized Physical Memory Manager\n";
}
}  // namespace kernel::memory::physical