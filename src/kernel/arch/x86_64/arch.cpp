#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <arch.hpp>
#include <drivers/serials.hpp>
#include <system/system.hpp>
#include "printf/printf.h"

using namespace kernel::drivers;

namespace kernel::arch {
void init() {
    serial::init(serial::COM1);

    system::init();
}
}  // namespace kernel::arch

size_t println(const char* str) {
    size_t length = strlen(str);

    return serial::write(serial::COM1, str, length);
}

void printc(char c) {
    serial::putc(serial::COM1, c);
}

extern "C" void putchar_(char c) {
    printc(c);
}