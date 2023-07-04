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

namespace kernel::arch {
void early_init();
void init();
uint64_t time_ns();
uint64_t epoch();
}  // namespace kernel::arch

struct regs {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;

    uint64_t int_no;
    uint64_t error_code;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed));

extern "C" {
size_t println(const char* str);
void printc(char c);
}