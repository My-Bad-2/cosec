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
struct Entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t flags;
    uint8_t limit_high : 4;
    uint8_t granularity : 4;
    uint8_t base_high;

    void set(uint32_t base = 0, uint32_t limit = 0, uint8_t granularity = 0, uint8_t flags = 0);
} __attribute__((packed));

struct Descriptor {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct Tss_Entry {
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

struct Tss {
    uint32_t reserved;
    uint64_t rsp[3];
    uint64_t reserved0;
    uint64_t ist[7];
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t reserved3;
    uint16_t iopb_offset;
} __attribute__((packed));

struct Gdt {
    Entry entries[GDT_ENTRY_COUNT];
    Tss_Entry tss;
} __attribute__((packed));

void init();

void load_tss(Tss* tss);
extern "C" void tss_update();
}  // namespace system::gdt