#pragma once

#include <stdint.h>

namespace kernel::drivers::rtc {
enum cmos_regs {
    CMOS_RTC_SECOND = 0x00,
    CMOS_RTC_MINUTE = 0x02,
    CMOS_RTC_HOUR = 0x04,
    CMOS_RTC_DAY = 0x07,
    CMOS_RTC_MONTH = 0x08,
    CMOS_RTC_YEAR = 0x09,
    CMOS_RTC_CENURY = 0x32,

    CMOS_STATUS_A = 0x0A,
};

uint8_t get_time(cmos_regs regs);
uint8_t time();

void sleep(uint64_t sec);
}  // namespace kernel::drivers::rtc