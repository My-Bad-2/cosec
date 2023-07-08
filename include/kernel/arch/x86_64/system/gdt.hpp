#pragma once

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#define GDT_ENTRY_COUNT 5
#define GDT_SEGMENT     0b00010000
#define GDT_PRESENT     0b10000000
#define GDT_USER        0b01100000
#define GDT_EXECUTABLE  0b00001000
#define GDT_READ_WRITE  0b00000010

#define GDT_LONG_MODE_GRANULARITY 0b0010
#define GDT_FLAGS   0b1100

#define GDT_KERNEL_CODE 1
#define GDT_KERNEL_DATA 2
#define GDT_USER_DATA   3
#define GDT_USER_CODE   4

#define GDT_RING_3      3

namespace system::gdt {
struct gdt_entry_t {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags;
    uint8_t limit_high : 4;
    uint8_t granularity : 4;
    uint8_t base_high;

    void set(uint32_t base = 0, uint32_t limit = 0, uint8_t granularity = 0, uint8_t flags = 0);
} __attribute__((packed));

struct gdt_descriptor_t {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct tss_entry_t {
    uint16_t len;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags0;
    uint8_t flags1;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;

    void set(uintptr_t tss);
} __attribute__((packed));

struct gdt_t {
    gdt_entry_t entries[GDT_ENTRY_COUNT];
    tss_entry_t tss;
} __attribute__((packed));

extern gdt_t gdt;

void init();
extern "C" void gdt_update(uint64_t descriptor);
}  // namespace system::gdt