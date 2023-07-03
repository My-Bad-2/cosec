#include <system/gdt.hpp>
#include <system/idt.hpp>
#include <system/system.hpp>
#include <system/pic.hpp>
#include <system/lapic.hpp>
#include <system/ioapic.hpp>
#include <system/simd.hpp>

namespace system {
void init() {
    asm volatile("cli");
    gdt::init();
    idt::init();
    simd::init();
    pic::init();
    ioapic::init();
    asm volatile("sti");
}
}  // namespace system