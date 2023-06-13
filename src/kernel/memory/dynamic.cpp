#include <kernel.h>
#include <stddef.h>
#include <string.h>

#include <debug/log.hpp>

#include <memory/dynamic.hpp>
#include <memory/memory.hpp>
#include <memory/physical.hpp>

namespace kernel::memory::dynamic {
Slab_cache* create_slab_cache(const char* name, size_t slab_size, int flags) {
    Slab_cache* cache = physical::alloc<Slab_cache*>();

    if (!cache && (flags & SLAB_PANIC)) {
        log::error << "Slab Cache create " << name
                   << ": couldn't allocate memory\n";

        while (true) {
            asm("cli");
            asm("hlt");
        }
    }

    if (!cache) {
        return nullptr;
    }

    cache->name = name;
    cache->slab_size = slab_size;
    cache->bufctl_count_max =
        (physical::PAGE_SIZE - sizeof(Slab_cache)) / cache->slab_size;

    cache->slabs = nullptr;

    cache->grow(1, static_cast<Slab_flags>(flags));

    return cache;
}

void Slab_cache::destroy(Slab_flags flags) {
    this->slabs = this->slabs_head;

    for (;;) {
        if (!this->slabs) {
            break;
        }

        if ((this->slabs->bufctl_count != this->bufctl_count_max) &&
            (flags & SLAB_PANIC)) {
            log::error << "Slab cache destroy ('" << this->name
                       << "'): a slab wasn't completely free\n";

            while (true) {
                asm("cli");
                asm("hlt");
            }
        }

        if (this->slabs->bufctl_count != this->bufctl_count_max) {
            return;
        }

        physical::free(reinterpret_cast<void*>(this->slabs->freelist_head));

        this->slabs = this->slabs->next;
    }

    memset(this, 0, sizeof(Slab_cache));
    physical::free(this);
}

static Slab_bufctl* create_bufctl_buffer() {
    Slab_bufctl* bufctl = physical::alloc<Slab_bufctl*>();

    if (!bufctl) {
        return nullptr;
    }

    bufctl->next = nullptr;
    return bufctl;
}

static void create_slab(Slab_cache* cache, Slab_bufctl* bufctl) {
    Slab* slab = reinterpret_cast<Slab*>(
        (reinterpret_cast<uintptr_t>(bufctl) + physical::PAGE_SIZE) -
        sizeof(Slab));

    slab->next = nullptr;
    slab->bufctl_count = cache->bufctl_count_max;
    slab->bufctl_addr = bufctl;
    slab->freelist_head = nullptr;
    slab->freelist = nullptr;

    if (!cache->slabs) {
        cache->slabs_head = slab;
        cache->slabs = slab;
    } else {
        cache->slabs->next = slab;
        cache->slabs = cache->slabs->next;
    }
}

void init_bufctl(Slab_cache* cache, Slab_bufctl* bufctl, size_t index,
                 Slab_flags flags) {
    Slab_bufctl* new_bufctl = reinterpret_cast<Slab_bufctl*>(
        reinterpret_cast<uintptr_t>(bufctl) + cache->slab_size * index);
    new_bufctl->index = index;

    if ((flags & SLAB_NO_ALIGN) &&
        ((reinterpret_cast<uint64_t>(new_bufctl) & 0xFFF) == 0)) {
        cache->slabs->bufctl_count--;
        cache->bufctl_count_max--;

        return;
    }

    if (!cache->slabs->freelist) {
        cache->slabs->freelist_head = new_bufctl;
        cache->slabs->freelist = new_bufctl;
    } else {
        cache->slabs->freelist->next = new_bufctl;
        cache->slabs->freelist = cache->slabs->freelist->next;
    }
}

void Slab_cache::grow(size_t count, Slab_flags flags) {
    this->slabs = this->slabs_head;

    for (size_t i = 0; i < count; i++) {
        Slab_bufctl* bufctl = create_bufctl_buffer();

        if (!bufctl && (flags & SLAB_PANIC)) {
            log::error << "Slab Cache grow ('" << this->name
                       << "'): Could not create bufctl\n";

            while (true) {
                asm("cli");
                asm("hlt");
            }
        }

        if (!bufctl) {
            return;
        }

        create_slab(this, bufctl);

        size_t max_const = this->bufctl_count_max;

        for (size_t j = 0; j < max_const; j++) {
            init_bufctl(this, bufctl, j, flags);
        }
    }
}

void Slab_cache::reap(Slab_flags flags) {
    (void)flags;

    this->slabs = this->slabs_head;

    Slab* prev = this->slabs_head;

    for (;;) {
        if (!this->slabs) {
            return;
        }

        if (this->slabs->bufctl_count == this->bufctl_count_max) {
            if (this->slabs == this->slabs_head) {
                this->slabs_head = this->slabs->next;
            }

            prev->next = this->slabs->next;

            physical::free(this->slabs);
            this->slabs = prev->next;

            continue;
        }

        prev = this->slabs;
        this->slabs = this->slabs->next;
    }
}

void* Slab_cache::alloc(Slab_flags flags) {
    this->slabs = this->slabs_head;

    for (;;) {
        if (!this->slabs && (flags & SLAB_AUTO_GROW)) {
            this->grow(1, flags);

            return this->alloc(flags);
        }

        if (!this->slabs && (flags & SLAB_PANIC)) {
            log::error << "Slab cache alloc ('" << this->name
                       << "'): Couldn't find allocatable memory\n";

            while (true) {
                asm("cli");
                asm("hlt");
            }
        }

        if (!this->slabs) {
            return nullptr;
        }

        if (this->slabs->freelist_head) {
            break;
        }

        this->slabs = this->slabs->next;
    }

    void* ptr = this->slabs->freelist_head;

    this->slabs->freelist_head = this->slabs->freelist_head->next;
    this->slabs->bufctl_count--;

    return ptr;
}

void Slab_cache::free(void* ptr, Slab_flags flags) {
    this->slabs = this->slabs_head;

    for (;;) {
        if (!this->slabs && (flags & SLAB_PANIC)) {
            log::error
                << "Slab cache free ('" << this->name
                << "'): Couldn't find a slab for the to be freed pointer\n";

            while (true) {
                asm("cli");
                asm("hlt");
            }
        }

        if (!this->slabs) {
            return;
        }

        if (this->slabs->bufctl_count < this->bufctl_count_max) {
            break;
        }

        this->slabs = this->slabs->next;
    }

    Slab_bufctl* new_bufctl = reinterpret_cast<Slab_bufctl*>(ptr);
    new_bufctl->next = this->slabs->freelist_head;
    new_bufctl->index = (reinterpret_cast<uintptr_t>(new_bufctl) -
                         reinterpret_cast<uintptr_t>(this->slabs->bufctl_addr) /
                             this->slab_size);

    this->slabs->freelist_head = new_bufctl;
    this->slabs->bufctl_count++;
}
}  // namespace kernel::memory::dynamic