#include <stdint.h>
#include <system/pic.hpp>
#include <system/port.hpp>
#include <debug/log.hpp>

#define ICW1_ICW4 0x01       // Indicates that ICW4 will be present
#define ICW1_SINGLE 0x02     // Single (cascade) mode
#define ICW1_INTERVAL4 0x04  // Call address interval 4 (8)
#define ICW1_LEVEL 0x08      // Level triggered (edge) mode
#define ICW1_INIT 0x10       // Initialization - required!

#define ICW4_8086 0x01        // 8086/88 (MCS-80/85) mode
#define ICW4_AUTO 0x02        // Auto (normal) EOI
#define ICW4_BUF_SLAVE 0x08   // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C  // Buffered mode/master
#define ICW4_SFNM 0x10        // Special fully nested (not)

namespace system::pic {
void eoi(uint64_t int_no) {
    if (int_no >= 40) {
        port::out8(PIC2_COMMAND, PIC_EOI);
    }

    port::out8(PIC1_COMMAND, PIC_EOI);
}

void mask(uint8_t irq) {
    uint16_t port = PIC1_DATA;

    if (irq >= 8) {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t val = port::in8(port) | (1 << irq);

    port::out8(port, val);
}

void unmask(uint8_t irq) {
    uint16_t port = PIC1_DATA;

    if (irq >= 8) {
        port = PIC2_DATA;
        irq -= 8;
    }

    uint8_t val = port::in8(port) & ~(1 << irq);

    port::out8(port, val);
}

void disable() {
    port::out8(PIC2_DATA, 0xFF);
    port::out8(PIC1_DATA, 0xFF);
}

void init() {
    uint8_t a = port::in8(PIC1_DATA);
    uint8_t b = port::in8(PIC2_DATA);

    port::out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port::io_pause();
    port::out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    port::io_pause();

    port::out8(PIC1_DATA, 0x20);
    port::io_pause();
    port::out8(PIC2_DATA, 0x20);
    port::io_pause();

    port::out8(PIC1_DATA, 0x04);
    port::io_pause();
    port::out8(PIC2_DATA, 0x02);
    port::io_pause();

    port::out8(PIC1_DATA, ICW4_8086);
    port::io_pause();
    port::out8(PIC2_DATA, ICW4_8086);
    port::io_pause();

    port::out8(PIC1_DATA, a);
    port::out8(PIC2_DATA, b);

    log::info << "Initialized PIC\n";
}
}  // namespace system::pic