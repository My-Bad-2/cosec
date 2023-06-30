#include <stdint.h>
#include <drivers/acpi.hpp>
#include <drivers/rtc.hpp>
#include <system/port.hpp>

namespace kernel::drivers::rtc {
uint8_t bcd_to_bin(uint8_t value) {
    return (value >> 4) * 10 + (value & 15);
}

uint8_t read(uint8_t write) {
    using namespace system::port;
    out8(0x70, write);

    return bcd_to_bin(in8(0x71));
}

uint8_t century() {
    if (acpi::fadt_header && acpi::fadt_header->century == 0) {
        return 20;
    }

    return read(0x32);
}

uint8_t year() {
    return read(0x09);
}

uint8_t month() {
    return read(0x08);
}

uint8_t day() {
    return read(0x07);
}

uint8_t hour() {
    return read(0x04);
}

uint8_t minute() {
    return read(0x02);
}

uint8_t second() {
    return read(0x00);
}

uint8_t time() {
    return hour() * 3600 + minute() * 60 + second();
}

void sleep(uint64_t sec) {
    uint64_t lastsec = time();

    while (lastsec == time()) {
        asm volatile("pause");
    }

    lastsec = time() + sec;
    while (lastsec != time()) {
        asm volatile("pause");
    }
}
}  // namespace kernel::drivers::rtc