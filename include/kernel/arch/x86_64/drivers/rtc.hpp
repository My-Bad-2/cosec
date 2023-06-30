#pragma once

#include <stdint.h>

namespace kernel::drivers::rtc {
uint8_t century();
uint8_t year();
uint8_t month();
uint8_t day();
uint8_t hour();
uint8_t minute();
uint8_t second();
uint8_t time();

void sleep(uint64_t sec);
}  // namespace kernel::drivers::rtc