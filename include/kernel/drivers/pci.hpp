#pragma once

#include <stdint.h>
#include <common/list.hpp>
#include <drivers/acpi.hpp>

namespace kernel::drivers::pci {
struct pci_device_header_t {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
};

struct pci_header_t {
    pci_device_header_t header;
    uint32_t bar0;
    uint32_t bar1;
    uint32_t bar2;
    uint32_t bar3;
    uint32_t bar4;
    uint32_t bar5;
    uint32_t cardbus_cis_ptr;
    uint16_t subsystem_vendor_ID;
    uint16_t subsystem_ID;
    uint32_t expansion_ROM_base_addr;
    uint8_t capabilities_ptr;
    uint8_t rsv0;
    uint16_t rsv1;
    uint32_t rsv2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
};

struct pci_device_t {
    pci_header_t* header;
    uint16_t bus;
    uint16_t device;
    uint64_t function;
};

extern const char* device_classes[];
const char* get_vendor_name(uint16_t vendor_id);
const char* get_device_name(uint16_t vendor_ID, uint16_t device_ID);
const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code);
const char* get_prog_if_name(uint8_t class_code, uint8_t subclass_code,
                             uint8_t prog_if);

void write32(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset,
             uint32_t value);
void write16(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset,
             uint16_t value);
void write8(uint16_t bus, uint16_t device, uint16_t function, uint32_t offset,
            uint8_t value);

uint32_t read32(uint16_t bus, uint16_t device, uint16_t function,
                uint32_t offset);
uint16_t read16(uint16_t bus, uint16_t device, uint16_t function,
                uint32_t offset);
uint8_t read8(uint16_t bus, uint16_t device, uint16_t function,
              uint32_t offset);

int device_has_functions(uint16_t bus, uint16_t device);
pci_header_t get_device_header(uint16_t bus, uint16_t device,
                               uint16_t function);

void enumerate();

void enable_mmio(uint16_t bus, uint16_t device, uint16_t function);
void become_bus_master(uint16_t bus, uint16_t device, uint16_t function);

struct pci_driver {
    uint8_t class_;
    uint8_t subclass;
    uint8_t prog_if;
    bool use_class_subclass_prog_if;
    uint16_t vendor_id;
    uint16_t device_id;
    bool use_vendor_device_id;

    void (*load_driver)(pci_header_t* header, uint16_t bus, uint16_t device,
                        uint16_t function);
};

void register_driver(uint8_t class_, uint8_t subclass, uint8_t prog_if,
                     void (*load_driver)(pci_header_t* header, uint16_t bus,
                                         uint16_t device, uint16_t function));
void register_driver(uint16_t vendor_id, uint16_t device_id,
                     void (*load_driver)(pci_header_t* header, uint16_t bus,
                                         uint16_t device, uint16_t function));

extern list<pci_driver>* pci_drivers;
}  // namespace kernel::drivers::pci

namespace msi {
union control {
    struct {
        uint16_t msie : 1;
        uint16_t mmc : 3;
        uint16_t mme : 3;
        uint16_t c64 : 1;
        uint16_t pvm : 1;
        uint16_t reserved : 6;
    };

    uint16_t raw;
} __attribute__((packed));

union address {
    struct {
        uint32_t reserved : 2;
        uint32_t destination_mode : 1;
        uint32_t redirection_hint : 1;
        uint32_t reserved_0 : 8;
        uint32_t destination_id : 8;
        uint32_t base_address : 12;
    };

    uint32_t raw;
} __attribute__((packed));

union data {
    struct {
        uint32_t vector : 8;
        uint32_t delivery_mode : 3;
        uint32_t reserved : 3;
        uint32_t level : 1;
        uint32_t trigger_mode : 1;
        uint32_t reserved_0 : 16;
    };

    uint32_t raw;
} __attribute__((packed));
}  // namespace msi