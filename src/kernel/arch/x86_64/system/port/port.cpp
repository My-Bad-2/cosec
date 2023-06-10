#include <system/port.hpp>
#include <stdint.h>

namespace system::port {
uint8_t in8(uint16_t port) {
    uint8_t val;

    asm volatile("inb %1, %0" : "=a"(val) : "d"(port));

    return val;
}

void out8(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "d"(port));
}

uint16_t in16(uint16_t port) {
    uint16_t val;

    asm volatile("inw %1, %0" : "=a"(val) : "d"(port));

    return val;
}

void out16(uint16_t port, uint16_t val) {
    asm volatile("outw %0, %1" : : "a"(val), "d"(port));
}

uint32_t in32(uint16_t port) {
    uint32_t val;

    asm volatile("inl %1, %0" : "=a"(val) : "d"(port));

    return val;
}

void out32(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1" : : "a"(val), "d"(port));
}

void io_pause() {
    asm volatile("outb %al, $0x80");
}
}