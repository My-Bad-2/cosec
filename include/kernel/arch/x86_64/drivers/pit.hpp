#pragma once

#include <stdint.h>

namespace kernel::drivers::pit {
extern uint8_t vector;

uint64_t time_ms();
void msleep(uint64_t ms);

void init();
void handler();
}