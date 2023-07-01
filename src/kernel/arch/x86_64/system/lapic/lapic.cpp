#include <lai/host.h>
#include <stdint.h>

#include <common/mmio.hpp>

#include <drivers/acpi.hpp>
#include <debug/log.hpp>
#include <specs/acpi.hpp>

#include <system/cpu.hpp>
#include <system/lapic.hpp>
#include <system/port.hpp>

#include <memory/virtual.hpp>
#include <memory/memory.hpp>

#include <time/time.hpp>

namespace system::lapic {
bool lapic::x2apic_check() {
    uint32_t a, b, c, d;
    if (cpu::id(1, 0, a, b, c, d) == false) {
        return this->x2apic = false;
    }

    if (!(c & (1 << 21))) {
        return this->x2apic = false;
    }

    dmar_header* dmar = reinterpret_cast<dmar_header*>(laihost_scan("DMAR", 0));

    if (dmar == nullptr) {
        return this->x2apic = true;
    }

    if ((dmar->flags & (1 << 0)) && (dmar->flags & (1 << 1))) {
        return this->x2apic = false;
    }

    return this->x2apic = true;
}

uint32_t lapic::read(uint32_t reg) {
    if (this->x2apic) {
        return cpu::rdmsr(reg_to_x2apic(reg));
    }

    return mmio::read32(this->mmio_base + reg);
}

void lapic::write(uint32_t reg, uint64_t value) {
    if (this->x2apic) {
        cpu::wrmsr(reg_to_x2apic(reg), value);
    }

    mmio::write32(this->mmio_base + reg, value);
}

void lapic::timer_calibrate() {
    this->write(LAPIC_TDCR, 0x03);
    this->write(LAPIC_TICR, 0xFFFFFFFF);

    this->write(LAPIC_TIMER, this->read(LAPIC_TIMER) & ~(1 << 16));
    this->write(LAPIC_TIMER, this->read(LAPIC_TIMER) | (1 << 16));

    this->ticks_per_ms = (0xFFFFFFFF - this->read(LAPIC_TCCR)) / 10;
}

void lapic::init() {
    x2apic_check();

    uint64_t base = cpu::rdmsr(0x1B) | (1 << 11);

    if (this->x2apic) {
        base |= (1 << 10);
    }

    cpu::wrmsr(0x1B, base);

    uintptr_t phys_mmio_base = base & ~0xFFF;

    using namespace kernel::memory;
    this->mmio_base = to_higher_half(phys_mmio_base);

    this->id = (this->x2apic ? this->read(LAPIC_ID)
                             : (this->read(LAPIC_ID) >> 24) & 0xFF);

    this->write(LAPIC_TPR, 0);
    this->write(LAPIC_SVR, 0xFF | (1 << 8));

    log::info << "Initialized LAPIC!\n";
}

void lapic::ipi(uint32_t flags, uint32_t id) {
    if (this->x2apic) {
        this->write(LAPIC_ICRLO,
                    (static_cast<uint64_t>(id) << 32) | (1 << 14) | flags);
    } else {
        this->write(LAPIC_ICRHI, id << 24);
        this->write(LAPIC_ICRLO, flags);
    }
}

void lapic::eoi() {
    this->write(LAPIC_EOI, 0);
}

void lapic::timer(uint8_t vector, uint64_t ms, timer_modes mode) {
    if (this->ticks_per_ms == 0) {
        this->timer_calibrate();
    }

    uint64_t ticks = this->ticks_per_ms * ms;

    this->write(LAPIC_TDCR, 0x03);

    uint64_t value = this->read(LAPIC_TIMER) & ~(3 << 17);
    value |= static_cast<int>(mode) << 17;
    value &= 0xFFFFFF00;
    value |= vector;

    this->write(LAPIC_TIMER, value);
    this->write(LAPIC_TICR, ticks ? ticks : 1);
    this->write(LAPIC_TIMER, this->read(LAPIC_TIMER) & ~(1 << 16));
}
}  // namespace system::lapic
