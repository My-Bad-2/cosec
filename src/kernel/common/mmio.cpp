#include <common/mmio.hpp>

namespace mmio {
void write8(uintptr_t address, uint8_t val) {
    (*((volatile uint8_t*)address)) = val;
}

void write16(uintptr_t address, uint16_t val) {
    (*((volatile uint16_t*)address)) = val;
}

void write32(uintptr_t address, uint32_t val) {
    (*((volatile uint32_t*)address)) = val;
}

void write64(uintptr_t address, uint64_t val) {
    (*((volatile uint64_t*)address)) = val;
}

uint8_t read8(uintptr_t address) {
    return (*((volatile uint8_t*)address));
}

uint16_t read16(uintptr_t address) {
    return (*((volatile uint16_t*)address));
}

uint32_t read32(uintptr_t address) {
    return (*((volatile uint32_t*)address));
}

uint64_t read64(uintptr_t address) {
    return (*((volatile uint64_t*)address));
}
}  // namespace mmio