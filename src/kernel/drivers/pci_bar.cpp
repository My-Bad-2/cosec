#include <cstdint>
#include <drivers/pci_bar.hpp>

namespace kernel::drivers::pci {
void read_bar(uint32_t* mask, uint16_t bus, uint16_t device, uint16_t function,
              uint32_t offset) {
    uint32_t data = read32(bus, device, function, offset);
    write32(bus, device, function, offset, 0xFFFFFFFF);

    *mask = read32(bus, device, function, offset);
    write32(bus, device, function, offset, data);
}

pci_bar_t get_bar(uint32_t* bar0, int bar_num, uint16_t bus, uint16_t device,
                  uint16_t function) {
    pci_bar_t bar;
    uint32_t* bar_ptr =
        reinterpret_cast<uint32_t*>(bar0 + bar_num * sizeof(uint32_t));

    if (*bar_ptr) {
        uint32_t mask;
        read_bar(&mask, bus, device, function, bar_num * sizeof(uint32_t));

        if (*bar_ptr & 0x04) {
            bar.type = pci_bar_type_t::MMIO64;

            uint32_t* bar_ptr_high =
                reinterpret_cast<uint32_t*>(bar0 + bar_num * sizeof(uint32_t));
            uint32_t mask_high;

            read_bar(&mask_high, bus, device, function,
                     (bar_num * sizeof(uint32_t)) + 0x04);

            bar.mem_address =
                (static_cast<uint64_t>(*bar_ptr_high & ~0xF) << 32) |
                (*bar_ptr & ~0xF);
            bar.size =
                ((static_cast<uint64_t>(mask_high) << 32) | (mask & ~0xF)) + 1;
        } else if (*bar_ptr & 0x01) {
            bar.type = pci_bar_type_t::IO;
            bar.io_address = static_cast<uint16_t>(*bar_ptr & ~0x3);
            bar.size = static_cast<uint16_t>(~(mask & ~0x3) + 1);
        } else {
            bar.type = pci_bar_type_t::MMIO32;
            bar.mem_address = static_cast<uint64_t>(*bar_ptr & ~0xF);
            bar.size = ~(mask & ~0xF) + 1;
        }
    } else {
        bar.type = pci_bar_type_t::NONE;
    }

    return bar;
}

uint16_t get_io_port(pci_header_t* header, uint16_t bus, uint16_t device,
                     uint16_t function) {
    uint16_t port = 0;

    for (int i = 0; i < 6; i++) {
        pci_bar_t pci_bar = get_bar(&header->bar0, i, bus, device, function);

        if (pci_bar.type == pci_bar_type_t::IO) {
            port = pci_bar.io_address;
            break;
        }
    }

    return port;
}
}  // namespace kernel::drivers::pci