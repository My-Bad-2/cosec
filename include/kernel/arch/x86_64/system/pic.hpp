#pragma once

#include <stddef.h>
#include <stdint.h>

namespace system::pic {
enum commands {
    PIC1 = 0x20,
    PIC2 = 0xA0,
    PIC1_COMMAND = PIC1,
    PIC1_DATA = (PIC1 + 1),
    PIC2_COMMAND = PIC2,
    PIC2_DATA = (PIC2 + 1),
    PIC_EOI = 0x20,
};

void eoi(uint64_t int_no);
void mask(uint8_t irq);
void unmask(uint8_t irq);

void disable();
void init();
}