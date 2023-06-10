#include <stdint.h>
#include <stddef.h>

namespace system::port {
extern "C" {
    uint8_t in8(uint16_t port);
    void out8(uint16_t port, uint8_t val);

    uint16_t in16(uint16_t port);
    void out16(uint16_t port, uint16_t val);

    uint32_t in32(uint16_t port);
    void out32(uint16_t port, uint32_t val);

    void io_pause();
}
}