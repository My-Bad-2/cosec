#include <stdint.h>
#include <debug/log.hpp>
#include <system/gdt.hpp>

// external asm function helper
extern "C" {
void gdt_update(uint64_t descriptor);
}

namespace system::gdt {
static Tss tss = {0, {}, 0, {}, 0, 0, 0, 0};
static Gdt gdt = {};
static Descriptor desc = {sizeof(Gdt) - 1, reinterpret_cast<uint64_t>(&gdt)};

void Entry::set(uint32_t base, uint32_t limit, uint8_t granularity,
                uint8_t flags) {
    this->limit_low = static_cast<uint16_t>(limit & 0xFFFF);
    this->base_low = static_cast<uint16_t>(base & 0xFFFF);
    this->base_mid = static_cast<uint8_t>((base >> 16) & 0xFF);
    this->flags = flags;
    this->limit_high = static_cast<uint8_t>((limit >> 16) & 0x0F);
    this->granularity = granularity;
    this->base_high = static_cast<uint8_t>((base >> 24) & 0xFF);
}

void Tss_Entry::set(uintptr_t tss) {
    this->len = sizeof(Tss);
    this->base_low = static_cast<uint16_t>(tss & 0xFFFF);
    this->base_mid = static_cast<uint8_t>((tss >> 16) & 0xFF);
    this->flags0 = 0b10001001;
    this->flags1 = 0;
    this->base_high = static_cast<uint8_t>((tss >> 24) & 0xFF);
    this->base_upper = static_cast<uint8_t>(tss >> 32);
    this->reserved = 0;
}

void init() {
    gdt.entries[0].set();

    gdt.entries[GDT_KERNEL_CODE].set(
        0, 0, GDT_LONG_MODE_GRANULARITY,
        GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_EXECUTABLE);
    gdt.entries[GDT_KERNEL_DATA].set(
        0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE);

    gdt.entries[GDT_USER_DATA].set(
        0, 0, 0, GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_USER);
    gdt.entries[GDT_USER_CODE].set(
        0, 0, GDT_LONG_MODE_GRANULARITY,
        GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE | GDT_EXECUTABLE | GDT_USER);

    gdt.tss.set(reinterpret_cast<uintptr_t>(&tss));

    gdt_update(reinterpret_cast<uintptr_t>(&desc));

    log::info << "Initialized GDT!\n";
}

void load_tss(Tss* tss) {
    gdt.tss.set(reinterpret_cast<uintptr_t>(tss));

    tss_update();
}
}  // namespace system::gdt