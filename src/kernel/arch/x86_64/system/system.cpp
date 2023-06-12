#include <system/gdt.hpp>
#include <system/idt.hpp>
#include <system/system.hpp>

namespace system {
void init() {
    asm volatile("cli");
    gdt::init();
    idt::init();
    asm volatile("sti");
}
}  // namespace system