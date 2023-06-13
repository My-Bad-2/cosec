#include <cstddef>
#include <cstdint>
#include <debug/log.hpp>
#include <memory/dynamic.hpp>
#include <memory/heap.hpp>
#include <memory/memory.hpp>
#include <memory/physical.hpp>
#include <string.h>

#include <stdlib.h>

namespace kernel::memory::heap {
using namespace dynamic;

static Slab_cache* slab_caches[6];

void init() {
    slab_caches[0] = create_slab_cache(
        "heap slab size 16", 16, SLAB_PANIC | SLAB_AUTO_GROW | SLAB_NO_ALIGN);
    slab_caches[1] = create_slab_cache(
        "heap slab size 32", 32, SLAB_PANIC | SLAB_AUTO_GROW | SLAB_NO_ALIGN);
    slab_caches[2] = create_slab_cache(
        "heap slab size 64", 64, SLAB_PANIC | SLAB_AUTO_GROW | SLAB_NO_ALIGN);
    slab_caches[3] = create_slab_cache(
        "heap slab size 128", 128, SLAB_PANIC | SLAB_AUTO_GROW | SLAB_NO_ALIGN);
    slab_caches[4] = create_slab_cache(
        "heap slab size 256", 256, SLAB_PANIC | SLAB_AUTO_GROW | SLAB_NO_ALIGN);
    slab_caches[5] = create_slab_cache(
        "heap slab size 512", 512, SLAB_PANIC | SLAB_AUTO_GROW | SLAB_NO_ALIGN);

    log::info << "Intialized Heap\n";
}

static uint32_t next_power_of_two(uint32_t n) {
    if (n <= 16) {
        return 16;
    }

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;

    return n;
}

static size_t round_alloc_size(size_t size) {
    if (size <= 512) {
        return next_power_of_two(size);
    } else {
        return align_up(size, physical::PAGE_SIZE);
    }
}

static int64_t size_to_slab_cache_index(size_t size) {
    switch (size) {
        case 16:
            return 0;

        case 32:
            return 1;

        case 64:
            return 2;

        case 128:
            return 3;

        case 256:
            return 4;

        case 512:
            return 5;

        default:
            return -1;
    }
}

// math cache index to specific size
static int64_t slab_cache_index_to_size(size_t index) {
    switch (index) {
        case 0:
            return 16;

        case 1:
            return 32;

        case 2:
            return 64;

        case 3:
            return 128;

        case 4:
            return 256;

        case 5:
            return 512;

        default:
            return -1;
    }
}

extern "C" void* malloc(size_t size) {
    size_t new_size = round_alloc_size(size + sizeof(malloc_metadata));
    void* ptr;

    if (new_size <= 512) {
        int64_t index = size_to_slab_cache_index(new_size);

        if (index == -1) {
            return nullptr;
        }

        ptr = slab_caches[index]->alloc(dynamic::SLAB_PANIC);

        if (!ptr) {
            return nullptr;
        }

        malloc_metadata* metadata = reinterpret_cast<malloc_metadata*>(ptr);
        metadata->size = index;
    } else {
        int64_t page_count = new_size / physical::PAGE_SIZE;

        if (page_count == -1) {
            return nullptr;
        }

        ptr = physical::alloc(page_count);

        if (!ptr) {
            return nullptr;
        }

        malloc_metadata* metadata = reinterpret_cast<malloc_metadata*>(ptr);
        metadata->size = page_count;
    }

    Hex64 addr = reinterpret_cast<uintptr_t>(ptr) + sizeof(malloc_metadata) +
                 HEAP_START_ADDR;

    log::warn << "Malloc initialized at " << addr << '\n';

    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) +
                                   sizeof(malloc_metadata) + HEAP_START_ADDR);
}

extern "C" void* realloc(void* ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }

    if (!size) {
        free(ptr);
        return nullptr;
    }

    ptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) -
                                  HEAP_START_ADDR - sizeof(malloc_metadata));

    size_t rounded_new_size = round_alloc_size(size);
    size_t old_size = 0;

    if ((reinterpret_cast<uintptr_t>(ptr) & 0xFFF) != 0) {
        malloc_metadata* metadata = reinterpret_cast<malloc_metadata*>(ptr);
        size_t index = metadata->size;

        int64_t old_size_temp = slab_cache_index_to_size(index);

        if (old_size_temp == -1) {
            return nullptr;
        }

        old_size = reinterpret_cast<size_t>(old_size);
    } else {
        malloc_metadata* metadata = reinterpret_cast<malloc_metadata*>(ptr);
        size_t page_count = metadata->size;

        old_size = page_count / physical::PAGE_SIZE;
    }

    ptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) +
                                  sizeof(malloc_metadata) + HEAP_START_ADDR);

    if (old_size == rounded_new_size) {
        return ptr;
    }

    void* new_ptr = malloc(size);

    log::warn << "Malloc new pointer at " << new_ptr << "\n";

    if (!new_ptr) {
        return nullptr;
    }

    if (old_size > rounded_new_size) {
        memcpy(new_ptr, ptr, rounded_new_size);
    } else {
        memcpy(new_ptr, ptr, old_size);
    }

    free(ptr);
    return new_ptr;
}

extern "C" void free(void* ptr) {
    if (!ptr) {
        return;
    }

    ptr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) -
                                  HEAP_START_ADDR - sizeof(malloc_metadata));

    if ((reinterpret_cast<uint64_t>(ptr) & 0xFFF) != 0) {
        malloc_metadata* metadata = reinterpret_cast<malloc_metadata*>(ptr);
        size_t index = metadata->size;

        slab_caches[index]->free(metadata, dynamic::SLAB_PANIC);
    } else {
        malloc_metadata* metadata = reinterpret_cast<malloc_metadata*>(ptr);
        size_t page_count = metadata->size;

        physical::free(metadata, page_count);
    }
}
}  // namespace kernel::memory::heap

void* operator new(size_t size) {
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete(void* ptr, size_t) noexcept {
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    free(ptr);
}

void operator delete[](void* ptr, size_t) noexcept {
    free(ptr);
}