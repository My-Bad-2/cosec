#include <system/gdt.hpp>
#include <system/idt.hpp>
#include <system/system.hpp>
#include <system/pic.hpp>
#include <system/lapic.hpp>

namespace system {
void init() {
    asm volatile("cli");
    gdt::init();
    idt::init();
    pic::init();
    asm volatile("sti");
}
}  // namespace system