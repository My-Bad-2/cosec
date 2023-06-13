#pragma once

#include <stddef.h>
#include <stdint.h>

#define HEAP_MAX_SIZE (4 * GiB)
#define HEAP_START_ADDR 0xFFFF900000000000

namespace kernel::memory::heap {
struct malloc_metadata {
    uint16_t size;
};

void init();
}  // namespace kernel::memory::heap