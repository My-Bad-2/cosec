#include <stddef.h>
#include <stdint.h>

#include <drivers/serials.hpp>
#include <system/port.hpp>

#include <debug/log.hpp>

namespace kernel::drivers::serial {
using namespace system::port;

void write_reg(com_port port, com_register reg, uint8_t val) {
    out8(static_cast<uint16_t>(port) + static_cast<uint16_t>(reg), val);
}

uint8_t read_reg(com_port port, com_register reg) {
    return in8(static_cast<uint16_t>(port) + static_cast<uint16_t>(reg));
}

bool can_read(com_port port) {
    return read_reg(port, COM_LINE_STATUS) & COM_LINE_DATA_READY;
}

bool can_write(com_port port) {
    return read_reg(port, COM_LINE_STATUS) & COM_LINE_TRANSMITTER_BUF_EMPTY;
}

void wait_write(com_port port) {
    while (!can_write(port))
        ;
}

void wait_read(com_port port) {
    while (!can_read(port))
        ;
}

void putc(enum com_port port, char c) {
    wait_write(port);
    write_reg(port, COM_DATA, c);
}

char getc(com_port port) {
    wait_read(port);
    return read_reg(port, COM_DATA);
}

size_t write(com_port port, const void* buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        putc(port, reinterpret_cast<const char*>(buf)[i]);
    }

    return size;
}

char* read(com_port port, char* buffer, size_t size) {
    char current_char = '1';
    size_t read_count = 0;

    while (read_count < size && ((!current_char) == '\0')) {
        current_char = getc(port);
        buffer[read_count++] = current_char;
        size--;
    }

    buffer[read_count] = '\0';

    return buffer;
}

void init(com_port port) {
    write_reg(port, COM_INTERRUPT_IDENTIFICATOR, 0);

    // Turn on dlab for setting baud rate
    write_reg(port, COM_LINE_CONTROL, COM_DLAB_STATUS);

    // set bauds
    write_reg(port, COM_BAUD_RATE_LOW, 115200 / 9600);
    write_reg(port, COM_BAUD_RATE_HIGH, 0);

    // we want 8-bit chars + clear dlab
    write_reg(port, COM_LINE_CONTROL, COM_DATA_SIZE_8);

    // turn on communication + redirect UART interupt into ICU
    write_reg(port, COM_MODEM_CONTROL,
              COM_MODEM_DTR | COM_MODEM_RTS | COM_MODEM_OUT2);
    write_reg(port, COM_INTERRUPT, COM_INTERRUPT_WHEN_DATA_AVAILABLE);

    log::info << "Initialized Serials!\n";
}
}  // namespace kernel::drivers::serial