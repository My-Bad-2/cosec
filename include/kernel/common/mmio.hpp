#pragma once

#include <stdint.h>

namespace mmio {
void write8(uintptr_t address, uint8_t val);
void write16(uintptr_t address, uint16_t val);
void write32(uintptr_t address, uint32_t val);
void write64(uintptr_t address, uint64_t val);

uint8_t read8(uintptr_t address);
uint16_t read16(uintptr_t address);
uint32_t read32(uintptr_t address);
uint64_t read64(uintptr_t address);
}  // namespace mmio