#include <stdint.h>
#include <stddef.h>

namespace kernel::drivers::serial {
enum com_port {
    COM1 = 0x3F8,
    COM2 = 0x2F8,
    COM3 = 0x3E8,
    COM4 = 0x2E8,
};

enum com_register {
    COM_DATA = 0,
    COM_INTERRUPT,
    COM_INTERRUPT_IDENTIFICATOR,
    COM_LINE_CONTROL,
    COM_MODEM_CONTROL,
    COM_LINE_STATUS,
    COM_MODEM_STATUS,
    COM_SCRATCH_REGISTER,

    COM_BAUD_RATE_LOW = COM_DATA,
    COM_BAUD_RATE_HIGH = COM_INTERRUPT,
    COM_FIFO_CONTROLLER = COM_INTERRUPT_IDENTIFICATOR,
};

enum com_line_control_bit {
    COM_DATA_SIZE_5 = 0,
    COM_DATA_SIZE_6,
    COM_DATA_SIZE_7,
    COM_DATA_SIZE_8,

    COM_DLAB_STATUS = (1 << 7),
};

enum com_modem_bit {
    COM_MODEM_DTR = 1 << 0,
    COM_MODEM_RTS = 1 << 1,
    COM_MODEM_OUT1 = 1 << 2,
    COM_MODEM_OUT2 = 1 << 3,
    COM_MODEM_LOOPBACK = 1 << 4,
};

enum com_interrupt_enable_bit {
    COM_INTERRUPT_WHEN_DATA_AVAILABLE = 1 << 0,
    COM_INTERRUPT_WHEN_TRANSMITTER_EMPTY = 1 << 1,
    COM_INTERRUPT_WHEN_BREAK_ERROR = 1 << 2,
    COM_INTERRUPT_WHEN_STATUS_UPDATE = 1 << 3,
};

enum com_line_status_bit {
    COM_LINE_DATA_READY = 1 << 0,
    COM_LINE_OVERRUN_ERROR = 1 << 1,
    COM_LINE_PARITY_ERROR = 1 << 2,
    COM_LINE_FRAMING_ERROR = 1 << 3,
    COM_LINE_BREAK_INDICATOR = 1 << 4,
    COM_LINE_TRANSMITTER_BUF_EMPTY = 1 << 5,
    COM_LINE_TRANSMITTER_EMPTY = 1 << 6,
    COM_LINE_IMPENDING_ERROR = 1 << 7,
};

void write_reg(com_port port, com_register reg, uint8_t val);
uint8_t read_reg(com_port port, com_register reg);

bool can_read(com_port port);
bool can_write(com_port port);

void wait_write(com_port port);
void wait_read(com_port port);

void putc(enum com_port port, char c);
char getc(com_port port);

size_t write(com_port port, void const* buf, size_t size);
char* read(com_port port, char* buffer, size_t size);

void init(com_port port);
}