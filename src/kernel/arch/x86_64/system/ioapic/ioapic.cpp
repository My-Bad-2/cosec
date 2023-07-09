#include <kernel.h>
#include <stddef.h>
#include <stdint.h>

#include <common/mmio.hpp>
#include <debug/log.hpp>

#include <drivers/acpi.hpp>

#include <memory/memory.hpp>

#include <system/idt.hpp>
#include <system/interrupts.hpp>
#include <system/ioapic.hpp>
#include <system/pic.hpp>

#include <vector>

namespace system::ioapic {
static std::vector<ioapic> ioapics;
bool initialized = false;

uint32_t ioapic::read(uint32_t reg) {
    mmio::write32(this->mmio_base, reg);
    return mmio::read32(this->mmio_base + 0x10);
}

void ioapic::write(uint32_t reg, uint32_t value) {
    mmio::write32(this->mmio_base, reg);
    mmio::write32(this->mmio_base + 0x10, value);
}

uint64_t ioapic::read_entry(uint32_t i) {
    return this->read(this->entry(i)) |
           (static_cast<uint64_t>(this->read(this->entry(i) + 0x01)) << 32);
}

void ioapic::write_entry(uint32_t i, uint64_t value) {
    this->write(this->entry(i), static_cast<uint32_t>(value));
    this->write(this->entry(i) + 0x01, static_cast<uint32_t>(value >> 32));
}

ioapic::ioapic(uintptr_t phys_mmio_base, uint32_t gsi_base)
    : gsi_base(gsi_base) {
    using namespace kernel::memory;

    this->mmio_base = to_higher_half(phys_mmio_base);
    this->redirs = ((this->read(0x01) >> 16) & 0xFF) + 1;

    for (uint32_t i = 0; i < this->redirs; i++) {
        this->mask(i);
    }
}

void ioapic::set(uint8_t i, uint8_t vector, delivery_mode delivery,
                 dest_mode dest, uint16_t flags, uint8_t id) {
    uint64_t value = 0;

    value |= vector;
    value |= (uint8_t)delivery << 8;
    value |= (uint8_t)dest << 11;

    if (flags & ACTIVE_HIGH_LOW) {
        value |= (1 << 13);
    }

    if (flags & EDGE_LEVEL) {
        value |= (1 << 15);
    }

    if (flags & MASKED) {
        value |= (1 << 16);
    }

    value |= static_cast<uint64_t>(id) << 56;

    this->write_entry(i, value);
}

void ioapic::mask(uint8_t i) {
    this->write_entry(i, this->read_entry(i) | (1 << 16));
}

void ioapic::unmask(uint8_t i) {
    this->write_entry(i, this->read_entry(i) & ~(1 << 16));
}

ioapic* ioapic_for_gsi(uint32_t gsi) {
    for (auto& entry : ioapics) {
        auto [start, end] = entry.gsi_range();

        if (start <= gsi && end >= gsi) {
            return &entry;
        }
    }

    return nullptr;
}

static ioapic& _ioapic_for_gsi(uint32_t gsi) {
    ioapic* ret = ioapic_for_gsi(gsi);

    if (ret == nullptr) {
        log::error << "Couldn't find IOAPIC for GSI " << gsi << '\n';

        while (true) {
            asm volatile("cli");
            asm volatile("hlt");
        }
    }

    return *ret;
}

void set(uint32_t gsi, uint8_t vector, delivery_mode delivery, dest_mode dest,
         uint16_t flags, uint8_t id) {
    ioapic& entry = _ioapic_for_gsi(gsi);

    entry.set(gsi - entry.gsi_range().first, vector, delivery, dest, flags, id);
}

void mask(uint32_t gsi) {
    ioapic& entry = _ioapic_for_gsi(gsi);

    entry.mask(gsi - entry.gsi_range().first);
}

void unmask(uint32_t gsi) {
    ioapic& entry = _ioapic_for_gsi(gsi);

    entry.unmask(gsi - entry.gsi_range().first);
}

void mask_irq(uint8_t irq) {
    using namespace kernel::drivers;

    for (const auto& iso : acpi::iso) {
        if (iso.irq_source == irq) {
            mask(iso.gsi);
            return;
        }
    }

    mask(irq);
}

void unmask_irq(uint8_t irq) {
    using namespace kernel::drivers;

    for (const auto& iso : acpi::iso) {
        if (iso.irq_source == irq) {
            unmask(iso.gsi);
            return;
        }
    }

    mask(irq);
}

void init() {
    using namespace kernel::drivers;

    if (acpi::madt_header == nullptr) {
        log::error << "MADT table not found\n";
        return;
    }

    pic::disable();

    for (const auto& entry : acpi::ioapic) {
        ioapics.emplace_back(entry.address, entry.gsib);
    }

    auto redirect_isa_irq = [](size_t i) {
        for (const auto& iso : acpi::iso) {
            if (iso.irq_source == i) {
                set(iso.gsi, iso.irq_source + 0x20, delivery_mode::FIXED,
                    dest_mode::PHYSICAL, iso.flags | MASKED,
                    kernel::smp_request.response->bsp_lapic_id);
                idt::handlers[iso.irq_source + 0x20].reserve();
                return;
            }
        }

        set(i, i + 0x20, delivery_mode::FIXED, dest_mode::PHYSICAL, MASKED,
            kernel::smp_request.response->bsp_lapic_id);
        idt::handlers[i + 0x20].reserve();
    };

    if (acpi::madt_header->legacy_pic()) {
        for (size_t i = 0; i < 16; i++) {
            if (i == 2) {
                continue;
            }

            redirect_isa_irq(i);
        }
    }

    initialized = true;

    log::info << "Initialized IOAPIC!\n";
}
}  // namespace system::ioapic