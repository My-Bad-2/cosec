#pragma once

#include <stddef.h>
#include <stdint.h>

namespace system::cpu {
bool id(uint32_t leaf, uint32_t subleaf, uint32_t& eax, uint32_t& ebx,
        uint32_t& ecx, uint32_t& edx);

uint64_t rdmsr(uint32_t msr);
void wrmsr(uint32_t msr, uint64_t value);

void invlpg(uintptr_t addr);
void enable_pat();

uintptr_t read_cr3();
void write_cr3(uintptr_t value);
}  // namespace system::cpu