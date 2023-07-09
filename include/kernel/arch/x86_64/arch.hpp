#pragma once

#include <stddef.h>
#include <stdint.h>
#include <system/cpu.hpp>

#define pause system::cpu::pause
#define enable_interrupts system::cpu::enable_interrupts
#define disable_interrupts system::cpu::disable_interrupts
#define interrupt_state system::cpu::interrupt_state
#define halt system::cpu::halt
#define interrupt_toggle system::cpu::interrupt_toggle

typedef system::cpu::register_t register_t;

namespace kernel::arch {
void early_init();
void init();
uint64_t time_ns();
uint64_t epoch();
}  // namespace kernel::arch

extern "C" {
size_t println(const char* str);
void printc(char c);
}