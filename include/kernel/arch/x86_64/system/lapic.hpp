#pragma once

#include <stdint.h>

namespace system::lapic {
enum class timer_modes : uint8_t {
    ONESHOT,
    PERIODIC,
    TSC_DEADLINE,
};

enum regs {
    LAPIC_ID = 0X0020,       // Local APIC ID
    LAPIC_VER = 0X0030,      // Local APIC Version
    LAPIC_TPR = 0X0080,      // Task Priority
    LAPIC_APR = 0X0090,      // Arbitration Priority
    LAPIC_PPR = 0X00A0,      // Processor Priority
    LAPIC_EOI = 0X00B0,      // EOI
    LAPIC_RRD = 0X00C0,      // Remote Read
    LAPIC_LDR = 0X00D0,      // Logical Destination
    LAPIC_DFR = 0X00E0,      // Destination Format
    LAPIC_SVR = 0X00F0,      // Spurious Interrupt Vector
    LAPIC_ISR = 0X0100,      // In-Service (8 registers)
    LAPIC_TMR = 0X0180,      // Trigger Mode (8 registers)
    LAPIC_IRR = 0X0200,      // Interrupt Request (8 registers)
    LAPIC_ESR = 0X0280,      // Error Status
    LAPIC_ICRLO = 0X0300,    // Interrupt Command
    LAPIC_ICRHI = 0X0310,    // Interrupt Command [63:32]
    LAPIC_TIMER = 0X0320,    // LVT Timer
    LAPIC_THERMAL = 0X0330,  // LVT Thermal Sensor
    LAPIC_PERF = 0X0340,     // LVT Performance Counter
    LAPIC_LINT0 = 0X0350,    // LVT LINT0
    LAPIC_LINT1 = 0X0360,    // LVT LINT1
    LAPIC_ERROR = 0X0370,    // LVT Error
    LAPIC_TICR = 0X0380,     // Initial Count (for Timer)
    LAPIC_TCCR = 0X0390,     // Current Count (for Timer)
    LAPIC_TDCR = 0X03E0,     // Divide Configuration (for Timer)
};

class lapic {
   private:
    uint32_t ticks_per_ms = 0;
    uintptr_t mmio_base = 0;
    bool x2apic = false;

    bool x2apic_check();

    uint32_t reg_to_x2apic(uint32_t reg) { return (reg >> 4) + 0x800; }

    uint32_t read(uint32_t reg);
    void write(uint32_t reg, uint64_t value);

    void timer_calibrate();

   public:
    uint32_t id = 0;

    lapic() : ticks_per_ms(0), mmio_base(0), x2apic(false), id(0) {}

    void init();
    void ipi(uint32_t flags, uint32_t id);
    void eoi();

    void timer(uint8_t vector, uint64_t ms, timer_modes mode);
};
}  // namespace system::lapic