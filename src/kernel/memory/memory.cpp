#include <kernel.h>
#include <memory/heap.hpp>
#include <memory/memory.hpp>
#include <memory/physical.hpp>
#include <memory/virtual.hpp>

namespace kernel::memory {
uintptr_t hhdm_offset = 0;

void init() {
    hhdm_offset = hhdm_request.response->offset;

    physical::init();
    heap::init();
    // virt::init(); // Page fault, idk why?
}
}  // namespace kernel::memory