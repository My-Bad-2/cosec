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

uint8_t get_time(cmos_regs regs) {
    if (regs == CMOS_RTC_CENURY) {
        if (acpi::fadt_header && acpi::fadt_header->century == 0) {
            return 20;
        }
    }

    return read(regs);
}

uint8_t time() {
    return get_time(CMOS_RTC_HOUR) * 3600 + get_time(CMOS_RTC_MINUTE) * 60 +
           get_time(CMOS_RTC_SECOND);
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