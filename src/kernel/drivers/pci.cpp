#include <lai/host.h>
#include <cstdint>
#include <debug/log.hpp>
#include <drivers/pci.hpp>
#include <memory/virtual.hpp>
#include <memory/memory.hpp>
#include <specs/acpi.hpp>

namespace kernel::drivers::pci {
list<pci_driver>* pci_drivers = nullptr;

using namespace kernel::memory::virt;

void enumerate_function(uint64_t address, uint64_t function, uint16_t bus,
                        uint16_t device) {
    uint64_t offset = function << 12;
    uint64_t function_address = address + offset;

    // kernel_pagemap->map(memory::to_higher_half(function_address), function_address);

    pci_device_header_t* pci_device_header =
        reinterpret_cast<pci_device_header_t*>(function_address);

    if ((pci_device_header->device_id == 0) ||
        (pci_device_header->device_id == 0xFFFF)) {
        return;
    }

    // log::debug << "Vendor Name: "
    //            << get_vendor_name(pci_device_header->vendor_id) << "\n";
    // log::debug << "Device Name: "
    //            << get_device_name(pci_device_header->vendor_id,
    //                               pci_device_header->device_id)
    //            << "\n";
    // log::debug << "Device Class: " << device_classes[pci_device_header->class_]
    //            << "\n";
    // log::debug << "Subclass Name: "
    //            << get_subclass_name(pci_device_header->class_,
    //                                 pci_device_header->subclass)
    //            << "\n";
    // log::debug << "Prog Interface Name: "
    //            << get_prog_if_name(pci_device_header->class_,
    //                                pci_device_header->subclass,
    //                                pci_device_header->prog_if)
    //            << "\n";

    if (pci_drivers != nullptr) {
        log::debug << "Searching for PCI driver...\n";

        node<pci_driver>* driver = pci_drivers->find<pci_header_t*>(
            [](pci_header_t* header, node<pci_driver>* node) {
                if (node->data.use_class_subclass_prog_if) {
                    return ((header->header.class_ == node->data.class_ ||
                             node->data.class_ == 0) &&
                            (header->header.subclass == node->data.subclass ||
                             node->data.subclass == 0) &&
                            (header->header.prog_if == node->data.prog_if ||
                             node->data.prog_if == 0));
                } else if (node->data.use_vendor_device_id) {
                    return ((header->header.vendor_id == node->data.vendor_id ||
                             node->data.vendor_id == 0) &&
                            (header->header.device_id == node->data.device_id ||
                             node->data.device_id == 0));
                }

                log::error << "Driver doesn't use vendor/device id or "
                              "class/subclass/prog_if which is invalid\n";
                return false;
            },
            reinterpret_cast<pci_header_t*>(pci_device_header));

        if (driver != nullptr) {
            log::debug << "Found driver. Init function: "
                       << (void*)driver->data.load_driver << "\n";
            driver->data.load_driver(
                reinterpret_cast<pci_header_t*>(pci_device_header), bus, device,
                function);
        } else {
            log::debug << "No driver found\n";
        }
    }
}

void enumerate_device(uint64_t bus_address, uint64_t device, uint16_t bus) {
    uint64_t offset = device << 15;
    uint64_t device_address = bus_address + offset;

    // kernel_pagemap->map(memory::to_higher_half(device_address), device_address);

    pci_device_header_t* pci_device_header =
        reinterpret_cast<pci_device_header_t*>(device_address);

    if (pci_device_header->device_id == 0) {
        return;
    }

    if (pci_device_header->device_id == 0xFFFF) {
        return;
    }

    for (uint64_t function = 0; function < 8; function++) {
        enumerate_function(device_address, function, bus, device);
    }
}

void enumerate_bus(uint64_t base_address, uint64_t bus) {
    uint64_t offset = bus << 20;
    uint64_t bus_address = base_address + offset;

    // kernel_pagemap->map(memory::to_higher_half(bus_address), bus_address);

    pci_device_header_t* pci_device_header =
        reinterpret_cast<pci_device_header_t*>(base_address);

    if (pci_device_header->device_id == 0) {
        return;
    }

    if (pci_device_header->device_id == 0xFFFF) {
        return;
    }

    for (uint64_t device = 0; device < 32; device++) {
        enumerate_device(bus_address, device, bus);
    }
}

void enumerate_mcfg(acpi_mcfg_header_t* mcfg) {
    int entries = ((mcfg->header.length) - sizeof(acpi_mcfg_header_t)) /
                  sizeof(acpi_mcfg_entry_t);

    for (int t = 0; t < entries; t++) {
        acpi_mcfg_entry_t* entry = reinterpret_cast<acpi_mcfg_entry_t*>(
            reinterpret_cast<uint64_t>(mcfg) + sizeof(acpi_mcfg_header_t) +
            (sizeof(acpi_mcfg_entry_t) * t));

        for (uint64_t bus = entry->start_bus; bus < entry->end_bus; bus++) {
            enumerate_bus(entry->base_address, bus);
        }
    }
}

void enumerate_legacy() {
    for (uint16_t bus = 0; bus < 8; bus++) {
        for (uint16_t device = 0; device < 32; device++) {
            uint64_t num_functions = device_has_functions(bus, device) ? 8 : 1;

            for (uint64_t function = 0; function < num_functions; function++) {
                pci_header_t pci_header =
                    get_device_header(bus, device, function);

                if (pci_header.header.vendor_id == 0 ||
                    pci_header.header.vendor_id == 0xFFFF) {
                    continue;
                }

                // pci_device_header_t* pci_device_header = &pci_header.header;

                // log::debug << "Vendor Name: "
                //            << get_vendor_name(pci_device_header->vendor_id)
                //            << '\n';
                // log::debug << "Device Name: "
                //            << get_device_name(pci_device_header->vendor_id,
                //                               pci_device_header->device_id)
                //            << '\n';
                // log::debug << "Device Class: "
                //            << device_classes[pci_device_header->class_] << '\n';
                // log::debug << "Subclass Name: "
                //            << get_subclass_name(pci_device_header->class_,
                //                                 pci_device_header->subclass)
                //            << '\n';
                // log::debug << "Prog Interface Name: "
                //            << get_prog_if_name(pci_device_header->class_,
                //                                pci_device_header->subclass,
                //                                pci_device_header->prog_if)
                //            << '\n';

                pci_header_t* header = new pci_header_t;
                memcpy(header, &pci_header, sizeof(pci_header_t));

                if (pci_drivers != nullptr) {
                    log::debug << "Searching for PCI driver...\n";

                    node<pci_driver>* driver = pci_drivers->find<pci_header_t*>(
                        [](pci_header_t* header, node<pci_driver>* node) {
                            if (node->data.use_class_subclass_prog_if) {
                                return ((header->header.class_ ==
                                             node->data.class_ ||
                                         node->data.class_ == 0) &&
                                        (header->header.subclass ==
                                             node->data.subclass ||
                                         node->data.subclass == 0) &&
                                        (header->header.prog_if ==
                                             node->data.prog_if ||
                                         node->data.prog_if == 0));
                            } else if (node->data.use_vendor_device_id) {
                                return ((header->header.vendor_id ==
                                             node->data.vendor_id ||
                                         node->data.vendor_id == 0) &&
                                        (header->header.device_id ==
                                             node->data.device_id ||
                                         node->data.device_id == 0));
                            }

                            log::error
                                << "Driver doesn't use vendor/device id or "
                                   "class/subclass/prog_if which is invalid\n";
                            return false;
                        },
                        header);

                    if (driver != nullptr) {
                        log::debug << "Found driver. Init function: "
                                   << (void*)driver->data.load_driver << '\n';
                        driver->data.load_driver(header, bus, device, function);
                    } else {
                        log::debug << "No driver found\n";
                    }
                }
            }
        }
    }
}

void enumerate() {
    acpi_mcfg_header_t* mcfg_table =
        reinterpret_cast<acpi_mcfg_header_t*>(laihost_scan("MCFG", 0));

    if (mcfg_table == nullptr) {
        log::debug << "No MCFG found! Using PCI configuration space address "
                      "mechanism\n";
        enumerate_legacy();
    } else {
        log::debug << "\nFound MCFG\n";
        enumerate_mcfg(mcfg_table);
    }
}

void register_driver(uint8_t class_, uint8_t subclass, uint8_t prog_if,
                     void (*load_driver)(pci_header_t* header, uint16_t bus,
                                         uint16_t device, uint16_t function)) {
    log::debug << "Registering pci driver for class: " << class_
               << ", Subclass: " << subclass << ", prog_if: " << prog_if
               << '\n';

    pci_driver driver = {
        .class_ = class_,
        .subclass = subclass,
        .prog_if = prog_if,
        .use_class_subclass_prog_if = true,
        .load_driver = load_driver,
    };

    if (pci_drivers == nullptr) {
        pci_drivers = new list<pci_driver>(10);
    }

    pci_drivers->add(driver);
}

void register_driver(uint16_t vendor_id, uint16_t device_id,
                     void (*load_driver)(pci_header_t* header, uint16_t bus,
                                         uint16_t device, uint16_t function)) {
    log::debug << "Registering pci driver for vendor: " << vendor_id
               << ", device: " << device_id << '\n';
    pci_driver driver = {
        .vendor_id = vendor_id,
        .device_id = device_id,
        .use_vendor_device_id = true,
        .load_driver = load_driver,
    };

    if (pci_drivers == nullptr) {
        pci_drivers = new list<pci_driver>(10);
    }

    pci_drivers->add(driver);
}
}  // namespace kernel::drivers::pci