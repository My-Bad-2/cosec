#pragma once

#include <stddef.h>
#include <stdint.h>

namespace system::pic {
enum commands {
    PIC1 = 0x20, // I/O base address for master PIC
    PIC2 = 0xA0, // I/O base address for slave PIC
    PIC1_COMMAND = PIC1,
    PIC1_DATA = (PIC1 + 1),
    PIC2_COMMAND = PIC2,
    PIC2_DATA = (PIC2 + 1),
    PIC_EOI = 0x20,
};

/**
 * @brief This is issued to the PIC chips of an IRQ-based interrupt routine.
 * 
 * @param int_no interrupt number
 */
void eoi(uint64_t int_no);

/**
 * @brief Ignore the request and continue normal operation
 * 
 * @param irq 
 */
void mask(uint8_t irq);

/**
 * @brief Remove bit from IMR
 * 
 * @param irq 
 */
void unmask(uint8_t irq);

/**
 * @brief Disable the PIC, if you're using processor LAPIC and the IOAPIC
 * 
 */
void disable();

/**
 * @brief This command makes the PIC wait for 3-extra 
 *   "Initialization words" on the data port.
 * 
 */
void init();
}