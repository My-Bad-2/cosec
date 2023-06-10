#pragma once

#include <stddef.h>
#include <stdint.h>

#define IDT_ENTRY_COUNT 256

namespace system::idt {
enum {
    IDT_TRAP = 0xEF,
    IDT_USER = 0x60,
    IDT_GATE = 0x8E
};

struct Descriptor {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct Entry {
    uint16_t offset_low;
    uint16_t code_segment;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;

    void set(uintptr_t handler, uint8_t ist, uint8_t flags);
} __attribute__((packed));

struct Idt {
    Entry entries[IDT_ENTRY_COUNT];
} __attribute__((packed));

void init();
extern "C" void idt_update(Descriptor* descriptor);
}