#include <stdint.h>

#include <debug/log.hpp>

#include <system/pic.hpp>
#include <system/port.hpp>

/**
 * Reinitialize the PIC controllers, giving them specified vector offsets
 * rather than 8h and 70h, as configured by default
 */
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
    /**
     * If the IRQ came from the Master PIC, it is sufficient to 
     * issue this command only to the Master PIC; however if the IRQ
     * came from the Slave PIC, it is necessary to issue the command
     * to both PIC chips.
     */

    if (int_no >= 40) {
        port::out8(PIC2_COMMAND, PIC_EOI);
    }

    port::out8(PIC1_COMMAND, PIC_EOI);
}

void mask(uint8_t irq) {
    /**
     * The PIC has an internal register called the IMR(Interrupt mask register).
     * It's 8-bits wide. This register is a bitmap of the request lines going
     * into the PIC. When a bit is set, the PIC ignores the request and continues
     * normal operation.
     *
     * Note that setting the mask on a higher request line will not affect a lower
     * line. Masking IRQ2 will cause the Slave PIC to stop raising IRQs.
     */
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
    /**
     * When you enter protected mode the first command you will need to give
     * the two PICs is the initialize command (code 0x11). This command makes
     * the PIC wait for 3 extra "initialization words" on the data port. These
     * bytes give the PIC :
     *      1) Its vector offset (ICW2)
     *      2) Tell it how it is wired to master/slaves (ICW3)
     *      3) Gives additional information about the environment (ICW4)
     */

    // save masks
    uint8_t a = port::in8(PIC1_DATA);
    uint8_t b = port::in8(PIC2_DATA);

    // starts the initialization sequence (in cascade mode)
    port::out8(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    port::io_pause();
    port::out8(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    port::io_pause();

    // ICW2: Master PIC vector offset
    port::out8(PIC1_DATA, 0x20);
    port::io_pause();

    // ICW2: Slave PIC vector offset
    port::out8(PIC2_DATA, 0x20);
    port::io_pause();

    // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    port::out8(PIC1_DATA, 0x04);
    port::io_pause();

    // ICW3: tell Slave PIC its cascade identity (0000 0010)
    port::out8(PIC2_DATA, 0x02);
    port::io_pause();

    // ICW4: have the PICs use 8086 mode (and not 8080 mode)
    port::out8(PIC1_DATA, ICW4_8086);
    port::io_pause();
    port::out8(PIC2_DATA, ICW4_8086);
    port::io_pause();

    // Restore saved masks
    port::out8(PIC1_DATA, a);
    port::out8(PIC2_DATA, b);

    log::info << "Initialized PIC!\n";
}
}  // namespace system::pic