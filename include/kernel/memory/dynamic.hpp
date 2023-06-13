#pragma once

#include <stddef.h>
#include <stdint.h>

namespace kernel::memory::dynamic {
enum Slab_flags {
    SLAB_PANIC = (1 << 0),
    SLAB_AUTO_GROW = (1 << 1),
    SLAB_NO_ALIGN = (1 << 2),
};

struct Slab_bufctl {
    Slab_bufctl* next;
    uint16_t index;
} __attribute__((packed));

struct Slab {
    Slab* next;
    size_t bufctl_count;
    void* bufctl_addr;

    Slab_bufctl* freelist_head;
    Slab_bufctl* freelist;
};

struct Slab_cache {
    const char* name;
    size_t slab_size;
    size_t bufctl_count_max;

    Slab* slabs_head;
    Slab* slabs;

    void destroy(Slab_flags flags);
    void* alloc(Slab_flags flags);
    void free(void* ptr, Slab_flags flags);
    void grow(size_t count, Slab_flags flags);
    void reap(Slab_flags flags);
};

Slab_cache* create_slab_cache(const char* name, size_t slab_size, int flags);
}  // namespace kernel::memory::dynamic