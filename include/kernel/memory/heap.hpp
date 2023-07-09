#pragma once

#include <stddef.h>
#include <stdint.h>
#include <frg/manual_box.hpp>
#include <mutex>

namespace kernel::memory::heap {
struct slab_t {
    std::mutex lock;
    uintptr_t first_free;
    size_t entry_size;

    void init(size_t size);
    void* alloc();
    void free(void* ptr);
};

struct slab_header_t {
    slab_t* slab;
};

struct big_alloc_meta_t {
    size_t pages;
    size_t size;
};

class slab_alloc_t {
   private:
    std::mutex lock;
    
    slab_t* get_slab(size_t size);

    void* big_malloc(size_t size);
    void* big_realloc(void* ptr, size_t size);
    void big_free(void* ptr);
    size_t big_alloc_size(void* ptr);

   public:
    slab_t slabs[10];

    slab_alloc_t();

    void* malloc(size_t size);
    void* calloc(size_t num, size_t size);
    void* realloc(void* ptr, size_t size);
    void free(void* ptr);
    size_t alloc_size(void* ptr);

    template <typename T = void*>
    T calloc(size_t num, size_t size) {
        return reinterpret_cast<T>(this->calloc(num, size));
    }

    template <typename T = void*>
    T malloc(size_t size) {
        return reinterpret_cast<T>(this->malloc(size));
    }

    template <typename T>
    T realloc(T ptr, size_t size) {
        return reinterpret_cast<T>(
            this->realloc(reinterpret_cast<void*>(ptr), size));
    }

    void free(auto ptr) { this->free(reinterpret_cast<void*>(ptr)); }

    size_t alloc_size(auto ptr) {
        return this->alloc_size(reinterpret_cast<void*>(ptr));
    }
};

extern frg::manual_box<slab_alloc_t> allocator;

void init();
}  // namespace kernel::memory::heap