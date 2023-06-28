#include <cstdint>
#include <drivers/pci.hpp>
#include <system/port.hpp>

#define COMMAND_PORT 0xCF8
#define DATA_PORT 0XCFC

namespace kernel::drivers::pci {
using namespace system::port;

uint32_t read32(uint16_t bus, uint16_t device, uint16_t function,
                uint32_t offset) {
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function << 0x07) << 8) | (offset & 0xFC);

    out32(COMMAND_PORT, id);
    uint32_t result = in32(DATA_PORT);

    return result >> (8 * (offset % 4));
}

uint16_t read16(uint16_t bus, uint16_t device, uint16_t function,
                uint32_t offset) {
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function << 0x07) << 8) | (offset & 0xFC);
    out32(COMMAND_PORT, id);
    uint16_t result = in16(DATA_PORT);

    return result >> (8 * (offset % 4));
}

uint8_t read8(uint16_t bus, uint16_t device, uint16_t function,
              uint32_t offset) {
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function << 0x07) << 8) | (offset & 0xFC);

    out32(COMMAND_PORT, id);
    uint8_t result = in8(DATA_PORT);

    return result >> (8 * (offset % 4));
}

void write32(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset,
             uint32_t value) {
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function << 0x07) << 8) | (offset & 0xFC);

    out32(COMMAND_PORT, id);
    out32(DATA_PORT, value);
}

void write16(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset,
             uint16_t value) {
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function << 0x07) << 8) | (offset & 0xFC);

    out32(COMMAND_PORT, id);
    out16(DATA_PORT, value);
}

void write8(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset,
            uint8_t value) {
    uint32_t id = (0x1 << 31) | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) |
                  ((function << 0x07) << 8) | (offset & 0xFC);

    out32(COMMAND_PORT, id);
    out8(DATA_PORT, value);
}

int device_has_functions(uint16_t bus, uint16_t device) {
    return read32(bus, device, 0, 0xE) & (1 << 7);
}

pci_header_t get_device_header(uint16_t bus, uint16_t device,
                               uint16_t function) {
    pci_header_t result;

    result.header.vendor_id = read32(bus, device, function, 0);
    result.header.vendor_id = read32(bus, device, function, 2);
    result.header.command = read32(bus, device, function, 4);
    result.header.status = read32(bus, device, function, 6);
    result.header.revision_id = read32(bus, device, function, 8);
    result.header.prog_if = read32(bus, device, function, 9);
    result.header.subclass = read32(bus, device, function, 0xA);
    result.header.class_ = read32(bus, device, function, 0xB);
    result.header.cache_line_size = read32(bus, device, function, 16);
    result.header.latency_timer = read32(bus, device, function, 18);
    result.header.header_type = read32(bus, device, function, 20);
    result.header.bist = read32(bus, device, function, 22);

    result.bar0 = read32(bus, device, function, 0x10);
    result.bar1 = read32(bus, device, function, 0x14);
    result.bar2 = read32(bus, device, function, 0x18);
    result.bar3 = read32(bus, device, function, 0x1C);
    result.bar4 = read32(bus, device, function, 0x20);
    result.bar5 = read32(bus, device, function, 0x24);

    result.cardbus_cis_ptr = read32(bus, device, function, 0x2C);

    result.subsystem_vendor_ID = read32(bus, device, function, 0X2E);
    result.subsystem_ID = read32(bus, device, function, 0X2F);
    result.expansion_ROM_base_addr = read32(bus, device, function, 0x30);

    result.interrupt_line = read32(bus, device, function, 0X3C);
    result.interrupt_pin = read32(bus, device, function, 0X3D);

    result.min_grant = read32(bus, device, function, 0X3E);
    result.max_latency = read32(bus, device, function, 0X3F);

    return result;
}

void enable_mmio(uint16_t bus, uint16_t device, uint16_t function) {
    write32(bus, device, function, 0x4,
            read32(bus, device, function, 0x4) | (1 << 1));
}

void become_bus_master(uint16_t bus, uint16_t device, uint16_t function) {
    write32(bus, device, function, 0x4,
            read32(bus, device, function, 0x4) | (1 << 2));
}
}  // namespace kernel::drivers::pci