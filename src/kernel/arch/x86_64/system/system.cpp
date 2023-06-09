#include <system/gdt.hpp>
#include <system/system.hpp>

namespace system {
void init() {
    asm volatile("cli");
    gdt::init();
    asm volatile("sti");
}
}