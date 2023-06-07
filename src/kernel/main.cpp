#include <limine.h>

extern "C" void _start() {
    asm volatile ("hlt");
}