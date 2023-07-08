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

struct idt_descriptor_t {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct idt_entry_t {
    uint16_t offset_low;
    uint16_t code_segment;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;

    void set(uintptr_t handler, uint8_t ist, uint8_t flags);
} __attribute__((packed));

struct idt_t {
    idt_entry_t entries[IDT_ENTRY_COUNT];
} __attribute__((packed));

extern idt_t idt;

void mask(uint8_t irq);
void unmask(uint8_t irq);

void init();
extern "C" void idt_update(idt_descriptor_t* descriptor);
}