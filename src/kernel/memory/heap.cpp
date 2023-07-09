#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <debug/log.hpp>

#include <frg/manual_box.hpp>

#include <memory/heap.hpp>
#include <memory/memory.hpp>
#include <memory/physical.hpp>

#include <mutex>

namespace kernel::memory::heap {
frg::manual_box<slab_alloc_t> allocator;

void slab_t::init(size_t size) {
    this->entry_size = size;
    this->first_free = to_higher_half(physical::alloc<uintptr_t>());

    uintptr_t avail =
        0x1000 - align_up(sizeof(slab_header_t), this->entry_size);
    auto slab_ptr = reinterpret_cast<slab_header_t*>(this->first_free);

    slab_ptr->slab = this;
    this->first_free += align_up(sizeof(slab_header_t), this->entry_size);

    auto array = reinterpret_cast<uint64_t*>(this->first_free);
    uintptr_t max = avail / this->entry_size - 1;
    uintptr_t factor = this->entry_size / 8;

    for (size_t i = 0; i < max; i++) {
        array[i * factor] =
            reinterpret_cast<uint64_t>(&array[(i + 1) * factor]);
    }

    array[max * factor] = 0;
}

void* slab_t::alloc() {
    std::unique_lock guard(this->lock);

    if (this->first_free == 0) {
        this->init(this->entry_size);
    }

    auto oldfree = reinterpret_cast<uint64_t*>(this->first_free);
    this->first_free = oldfree[0];
    memset(oldfree, 0, this->entry_size);

    return oldfree;
}

void slab_t::free(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    std::unique_lock guard(this->lock);

    auto new_head = static_cast<uint64_t*>(ptr);
    new_head[0] = this->first_free;
    this->first_free = reinterpret_cast<uintptr_t>(new_head);
}

slab_alloc_t::slab_alloc_t() {
    this->slabs[0].init(8);
    this->slabs[1].init(16);
    this->slabs[2].init(24);
    this->slabs[3].init(32);
    this->slabs[4].init(48);
    this->slabs[5].init(64);
    this->slabs[6].init(128);
    this->slabs[7].init(256);
    this->slabs[8].init(512);
    this->slabs[9].init(1024);
}

slab_t* slab_alloc_t::get_slab(size_t size) {
    for (slab_t& slab : this->slabs) {
        if (slab.entry_size >= size) {
            return &slab;
        }
    }

    return nullptr;
}

void* slab_alloc_t::big_malloc(size_t size) {
    size_t pages = div_roundup(size, physical::PAGE_SIZE);
    void* ptr = to_higher_half(physical::alloc(pages + 1));

    auto metadata = reinterpret_cast<big_alloc_meta_t*>(ptr);
    metadata->pages = pages;
    metadata->size = size;

    return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ptr) +
                                   physical::PAGE_SIZE);
}

void* slab_alloc_t::big_realloc(void* ptr, size_t size) {
    if (ptr == nullptr) {
        return this->malloc(size);
    }

    big_alloc_meta_t* metadata = reinterpret_cast<big_alloc_meta_t*>(
        reinterpret_cast<uintptr_t>(ptr) - 0x1000);
    size_t old_size = metadata->size;

    if (div_roundup(old_size, physical::PAGE_SIZE) ==
        div_roundup(size, physical::PAGE_SIZE)) {
        metadata->size = size;
        return ptr;
    }

    if (size == 0) {
        this->free(ptr);
        return nullptr;
    }

    if (size < old_size) {
        old_size = size;
    }

    void* new_ptr = this->malloc(size);
    if (new_ptr == nullptr) {
        return ptr;
    }

    memcpy(new_ptr, ptr, old_size);
    this->free(ptr);

    return new_ptr;
}

void slab_alloc_t::big_free(void* ptr) {
    auto metadata = reinterpret_cast<big_alloc_meta_t*>(
        reinterpret_cast<uintptr_t>(ptr) - physical::PAGE_SIZE);
    physical::free(from_higher_half(metadata), metadata->pages + 1);
}

size_t slab_alloc_t::big_alloc_size(void* ptr) {
    return reinterpret_cast<big_alloc_meta_t*>(
               reinterpret_cast<uintptr_t>(ptr) - physical::PAGE_SIZE)
        ->size;
}

void* slab_alloc_t::malloc(size_t size) {
    slab_t* slab = this->get_slab(size);

    if (slab == nullptr) {
        return this->big_malloc(size);
    }

    return slab->alloc();
}

void* slab_alloc_t::calloc(size_t num, size_t size) {
    void* ptr = this->malloc(num * size);

    if (ptr == nullptr) {
        return nullptr;
    }

    memset(ptr, 0, num * size);

    return ptr;
}

void* slab_alloc_t::realloc(void* ptr, size_t size) {
    if (ptr == nullptr) {
        return this->malloc(size);
    }

    if ((reinterpret_cast<uintptr_t>(ptr) & 0xFFF) == 0) {
        return this->big_realloc(ptr, size);
    }

    slab_t* slab = reinterpret_cast<slab_header_t*>(
                       reinterpret_cast<uintptr_t>(ptr) & ~0xFFF)
                       ->slab;
    size_t old_size = slab->entry_size;

    if (size == 0) {
        this->free(ptr);
        return nullptr;
    }

    if (size < old_size) {
        old_size = size;
    }

    void* new_ptr = this->malloc(size);
    if (new_ptr == nullptr) {
        return ptr;
    }

    memcpy(new_ptr, ptr, old_size);
    this->free(ptr);

    return new_ptr;
}

void slab_alloc_t::free(void* ptr) {
    if (ptr == nullptr) {
        return;
    }

    if ((reinterpret_cast<uintptr_t>(ptr) & 0xFFF) == 0) {
        return this->big_free(ptr);
    }

    reinterpret_cast<slab_header_t*>(reinterpret_cast<uintptr_t>(ptr) & ~0xFFF)
        ->slab->free(ptr);
}

size_t slab_alloc_t::alloc_size(void* ptr) {
    if (ptr == nullptr) {
        return 0;
    }

    if ((reinterpret_cast<uintptr_t>(ptr) & 0xFFF) == 0) {
        return this->big_alloc_size(ptr);
    }

    return reinterpret_cast<slab_header_t*>(reinterpret_cast<uintptr_t>(ptr) &
                                            ~0xFFF)
        ->slab->entry_size;
}

void init() {
    allocator.initialize();

    log::info << "Initialized Slab allocator!\n";
}
}  // namespace kernel::memory::heap