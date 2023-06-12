#include <stdint.h>
#include <debug/log.hpp>
#include <system/gdt.hpp>
#include <system/idt.hpp>

extern "C" uintptr_t __interrupt_vector[256];

namespace system::idt {
static Idt idt = {};
static Descriptor desc = {
    sizeof(Idt) - 1,
    reinterpret_cast<uintptr_t>(&idt),
};

void Entry::set(uintptr_t handler, uint8_t ist, uint8_t flags) {
    this->offset_low = handler;
    this->code_segment = GDT_KERNEL_CODE * 8;
    this->ist = ist;
    this->attributes = flags;
    this->offset_mid = handler >> 16;
    this->offset_high = handler >> 32;
    this->zero = 0;
}

void init() {
    for (int i = 0; i < 256; i++) {
        idt.entries[i].set(__interrupt_vector[i], 0, IDT_GATE);
    }

    idt.entries[32].set(__interrupt_vector[32], 0, IDT_GATE);

    idt_update(&desc);

    log::info << "Initialized IDT!\n";
}
}  // namespace system::idt