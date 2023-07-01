#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <printf/printf.h>
#include <arch.hpp>

#include <drivers/hpet.hpp>
#include <drivers/pit.hpp>
#include <drivers/rtc.hpp>
#include <drivers/serials.hpp>

#include <common/math.h>
#include <system/system.hpp>
#include <time/time.hpp>

#include <console/console.hpp>

using namespace kernel::drivers;

namespace kernel::arch {
void early_init() {
    serial::init(serial::COM1);
}

void init() {
    system::init();
    drivers::pit::init();
    drivers::hpet::init();
}

uint64_t time_ns() {
    if (drivers::hpet::initialized) {
        return drivers::hpet::time_ns();
    }

    return 0;
}

uint64_t epoch() {
    using namespace kernel::drivers::rtc;
    return ::epoch(second(), minute(), hour(), day(), month(), year(),
                   century());
}
}  // namespace kernel::arch

size_t println(const char* str) {
    size_t length = strlen(str);

    if (kernel::console::initialized) {
        kernel::console::write(str);
    }

    return serial::write(serial::COM1, str, length);
}

void printc(char c) {
    if (kernel::console::initialized) {
        kernel::console::printc(c);
    }

    serial::putc(serial::COM1, c);
}

extern "C" void putchar_(char c) {
    printc(c);
}