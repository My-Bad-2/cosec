#include <system/gdt.hpp>
#include <system/idt.hpp>
#include <system/ioapic.hpp>
#include <system/lapic.hpp>
#include <system/pic.hpp>
#include <system/simd.hpp>
#include <system/system.hpp>

namespace system {
void init() {
    asm volatile("cli");

    gdt::init();
    idt::init();
    pic::init();

    simd::init();

    lapic::lapic_.init();
    ioapic::init();

    asm volatile("sti");
}
}  // namespace system