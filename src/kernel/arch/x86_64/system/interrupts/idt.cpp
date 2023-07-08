#include <stdint.h>
#include <debug/log.hpp>

#include <system/gdt.hpp>
#include <system/idt.hpp>
#include <system/ioapic.hpp>
#include <system/pic.hpp>

#include <drivers/acpi.hpp>

extern "C" uintptr_t __interrupt_vector[256];

namespace system::idt {
idt_t idt = {};
static idt_descriptor_t desc = {
    sizeof(idt_t) - 1,
    reinterpret_cast<uintptr_t>(&idt),
};

void idt_entry_t::set(uintptr_t handler, uint8_t ist, uint8_t flags) {
    this->offset_low = handler;
    this->code_segment = GDT_KERNEL_CODE * 8;
    this->ist = ist;
    this->attributes = flags;
    this->offset_mid = handler >> 16;
    this->offset_high = handler >> 32;
    this->zero = 0;
}

using namespace kernel::drivers;

void mask(uint8_t irq) {
    if (ioapic::initialized && acpi::madt_header->legacy_pic()) {
        ioapic::mask_irq(irq);
    } else {
        pic::mask(irq);
    }
}

void unmask(uint8_t irq) {
    if (ioapic::initialized && acpi::madt_header->legacy_pic()) {
        ioapic::unmask_irq(irq);
    } else {
        pic::unmask(irq);
    }
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