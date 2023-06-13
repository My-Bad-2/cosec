#pragma once

#include <stddef.h>
#include <stdint.h>

namespace std {
template <typename T, typename U>
struct pair {
    T first;
    U second;
};
}  // namespace std

namespace kernel::memory::virt {
enum Flags {
    READ = (1 << 0),
    WRITE = (1 << 1),
    EXEC = (1 << 2),
    USER = (1 << 3),
    GLOBAL = (1 << 4),
    LPAGE = (1 << 5),
    LLPAGE = (1 << 6),

    RW = READ | WRITE,
    RWX = RW | EXEC,

    RWU = RW | USER,
    RWXU = RWX | USER,
};

#define is_lpage(x) ((x & LPAGE) || (x & LLPAGE))

enum Caching {
    UNCACHABLE = 0,
    WRITE_COMBINING = 0x01,
    WRITE_THROUGH = 0x04,
    WRITE_PROTECTED = 0x05,
    WRITE_BACK = 0x06,
    UNCACHED = 0x07,
    MMIO = UNCACHABLE,
    FRAMEBUFFER = WRITE_COMBINING,
};

constexpr inline Caching default_caching = WRITE_BACK;
constexpr inline Flags default_flags = RWX;

constexpr inline uint64_t KiB4 = 0x1000;
constexpr inline uint64_t MiB2 = 0x200000;
constexpr inline uint64_t GiB1 = 0x40000000;

constexpr inline uintptr_t invalid_addr = -1;

extern uintptr_t pa_mask;

struct pt_entry {
    uintptr_t value = 0;

    void set_flags(uintptr_t flags, bool enabled) {
        uintptr_t temp = this->value;

        temp &= ~flags;

        if (enabled) {
            temp |= flags;
        }

        this->value = temp;
    }

    bool get_flags(uintptr_t flags) {
        return (this->value & flags) ? true : false;
    }

    uintptr_t get_flags() { return this->value & ~pa_mask; }

    uintptr_t get_addr() { return this->value & pa_mask; }

    void set_addr(uintptr_t address) {
        uintptr_t temp = this->value;

        temp &= ~pa_mask;
        temp |= address;
        this->value = temp;
    }
};

struct pagetable;

struct pagemap {
    pagetable* toplvl;

    size_t llpage_size = 0;
    size_t lpage_size = 0;
    size_t page_size = 0;

    inline size_t get_psize(size_t flags) {
        size_t psize = this->page_size;

        if (flags & LPAGE) {
            psize = this->lpage_size;
        }

        if (flags & LLPAGE) {
            psize = this->llpage_size;
        }

        return psize;
    }

    uintptr_t map(uintptr_t virt_addr, uintptr_t phys_addr,
                  size_t flags = default_flags,
                  Caching cache = default_caching);
    bool unmap(uintptr_t virt_addr, size_t flags = 0);

    bool set_flags(uintptr_t virt_addr, size_t flags = 0,
                   Caching cache = default_caching);

    inline bool remap(uintptr_t virt_addr_old, uintptr_t virt_addr_new,
                      size_t flags = default_flags,
                      Caching cache = default_caching) {
        uintptr_t phys_addr = this->virt_to_phys(virt_addr_old, flags);
        this->unmap(virt_addr_old, flags);

        return this->map(virt_addr_new, phys_addr, flags, cache);
    }

    inline void map_range(uintptr_t virt_addr, uintptr_t phys_addr,
                          size_t count, size_t flags = default_flags,
                          Caching cache = default_caching) {
        size_t psize = this->get_psize(flags);

        for (size_t i = 0; i < count; i += psize) {
            this->map(virt_addr + i, phys_addr + i, flags, cache);
        }
    }

    inline void unmap_range(uintptr_t virt_addr, size_t count,
                            size_t flags = 0) {
        size_t psize = this->get_psize(flags);

        for (size_t i = 0; i < count; i += psize) {
            this->unmap(virt_addr + i, flags);
        }
    }

    inline void remap_range(uintptr_t virt_addr_old, uintptr_t virt_addr_new,
                            size_t count, size_t flags = default_flags,
                            Caching cache = default_caching) {
        size_t psize = this->get_psize(flags);

        for (size_t i = 0; i < count; i += psize) {
            this->remap(virt_addr_old + i, virt_addr_new + i, flags, cache);
        }
    }

    inline void set_flags_range(uintptr_t virt_addr, size_t count,
                                size_t flags = default_flags,
                                Caching cache = default_caching) {
        size_t psize = this->get_psize(flags);

        for (size_t i = 0; i < count; i += psize) {
            this->set_flags(virt_addr + i, flags, cache);
        }
    }

    inline std::pair<size_t, size_t> required_size(size_t size) {
        if (size >= this->llpage_size) {
            return {this->llpage_size, LLPAGE};
        } else if (size >= this->lpage_size) {
            return {this->lpage_size, LPAGE};
        }

        return {this->page_size, 0};
    }

    void load();
    void save();

    pt_entry* virt_to_pte(uintptr_t virt_addr, bool allocate, uintptr_t psize);
    uintptr_t virt_to_phys(uintptr_t virt_addr, size_t flags = 0);

    pagemap();
    ~pagemap();
};

extern pagemap* kernel_pagemap;

uintptr_t flags_to_arch(size_t flags);

bool is_canonical(uintptr_t addr);
void destroy_pagemap(pagemap* map);

[[gnu::weak]] void arch_init();

void init();
}  // namespace kernel::memory::virt