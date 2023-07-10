#include <stdint.h>
#include <debug/log.hpp>
#include <mutex>
#include <system/gdt.hpp>
#include <system/tss.hpp>

// external asm function helper

namespace system::gdt {
tss::tss_t tss = {0, {}, 0, {}, 0, 0, 0, 0};
gdt_t gdt = {};
gdt_descriptor_t desc = {sizeof(gdt_t) - 1, reinterpret_cast<uint64_t>(&gdt)};
static std::mutex lock;

void gdt_entry_t::set(uint32_t base, uint32_t limit, uint8_t granularity,
                      uint8_t flags) {
    // Set up the descriptor base address
    this->base_low = static_cast<uint16_t>(base & 0xFFFF);
    this->base_mid = static_cast<uint8_t>((base >> 16) & 0xFF);
    this->base_high = static_cast<uint8_t>((base >> 24) & 0xFF);

    // Set up descriptor limits
    this->limit_low = static_cast<uint16_t>(limit & 0xFFFF);
    this->limit_high = static_cast<uint8_t>((limit >> 16) & 0x0F);

    // Finally, set up the granularity and access flags
    this->granularity = granularity;
    this->flags = flags;
}

void tss_entry_t::set(uintptr_t tss) {
    this->len = sizeof(tss::tss_t);

    // Base address
    this->base_low = static_cast<uint16_t>(tss & 0xFFFF);
    this->base_mid = static_cast<uint8_t>((tss >> 16) & 0xFF);
    this->base_high = static_cast<uint8_t>((tss >> 24) & 0xFF);
    this->base_upper = static_cast<uint8_t>(tss >> 32);

    // Flags
    this->flags0 = 0b10001001;
    this->flags1 = 0;

    this->reserved = 0;
}

void init() {
    std::unique_lock guard(lock);

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

    tss::init(&tss, &gdt);

    log::info << "Initialized GDT!\n";
}
}  // namespace system::gdt