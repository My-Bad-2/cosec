#include <stdint.h>
#include <string.h>

#include <memory/memory.hpp>
#include <memory/physical.hpp>
#include <memory/virtual.hpp>
#include <system/gdt.hpp>
#include <system/tss.hpp>

#include <kernel.h>

#include <debug/log.hpp>

namespace system::tss {
extern "C" void load_tss(uintptr_t address, uintptr_t gdt, uint8_t selector);

void init(tss_t* tss, gdt::gdt_t* gdt) {
    using namespace kernel::memory;
    load_tss(reinterpret_cast<uintptr_t>(tss), reinterpret_cast<uintptr_t>(gdt),
             0x28);

    memset(tss, 0, sizeof(tss_t));

    // Set up interrupt stack tables
    tss->ist[0] =
        to_higher_half(reinterpret_cast<uint64_t>(physical::alloc(8)));
    tss->ist[1] =
        to_higher_half(reinterpret_cast<uint64_t>(physical::alloc(8)));
    tss->ist[2] =
        to_higher_half(reinterpret_cast<uint64_t>(physical::alloc(8)));

    memset(reinterpret_cast<void*>(tss->ist[0]), 0, physical::PAGE_SIZE);
    memset(reinterpret_cast<void*>(tss->ist[1]), 0, physical::PAGE_SIZE);
    memset(reinterpret_cast<void*>(tss->ist[2]), 0, physical::PAGE_SIZE);

    tss->ist[0] += physical::PAGE_SIZE * 8;
    tss->ist[1] += physical::PAGE_SIZE * 8;
    tss->ist[2] += physical::PAGE_SIZE * 8;

    asm volatile("mov %%rsp, %0" : "=r"(tss->rsp[0]));
    asm volatile("ltr %%ax" ::"a"(0x28));

    log::info << "Initialized TSS!\n";
}
}  // namespace system::tss