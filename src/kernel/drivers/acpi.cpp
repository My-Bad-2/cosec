#include <lai/core.h>
#include <lai/drivers/ec.h>
#include <lai/helpers/pm.h>
#include <lai/helpers/sci.h>
#include <lai/host.h>

#include <kernel.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <debug/log.hpp>

#include <memory/memory.hpp>
#include <memory/physical.hpp>
#include <memory/virtual.hpp>

#include <drivers/acpi.hpp>

namespace kernel::drivers::acpi {
acpi_fadt_t* fadt_header = nullptr;
acpi_madt_header_t* madt_header = nullptr;

std::vector<acpi_sdt_header_t*> tables;

std::vector<acpi_madt_lapic_t> lapic;
std::vector<acpi_madt_ioapic_t> ioapic;
std::vector<acpi_madt_iso_t> iso;
std::vector<acpi_madt_ionmi_t> ionmi;
std::vector<acpi_madt_lnmi_t> lnmi;
std::vector<acpi_madt_lapicao_t> lacpicao;
std::vector<acpi_madt_lx2apic_t> x2apic;

acpi_rsdp* rsdp = nullptr;
acpi_rsdt* rsdt = nullptr;
bool xsdt = false;

void* find_table(const char* signature, int index) {
    if (!strncmp(signature, "DSDT", 4)) {
        uintptr_t dsdt_address = 0;

        if (xsdt == true &&
            kernel::memory::virt::is_canonical(fadt_header->x_dsdt)) {
            dsdt_address = fadt_header->x_dsdt;
        } else {
            dsdt_address = fadt_header->dsdt;
        }

        return reinterpret_cast<void*>(
            kernel::memory::to_higher_half(dsdt_address));
    }

    for (const auto& header : tables) {
        if (!strncmp(reinterpret_cast<const char*>(header->signature),
                     signature, 4)) {
            if (index > 0) {
                index--;
            } else {
                return reinterpret_cast<void*>(header);
            }
        }
    }

    return nullptr;
}

void madt_init() {
    if (madt_header == nullptr) {
        return;
    }

    uintptr_t start = kernel::memory::to_higher_half(
        reinterpret_cast<uintptr_t>(madt_header->entries));
    uintptr_t end = kernel::memory::to_higher_half(
        reinterpret_cast<uintptr_t>(madt_header) + madt_header->sdt.length);

    acpi_madt_t* madt = reinterpret_cast<acpi_madt_t*>(start);

    for (uintptr_t entry = start; entry < end;
         entry += madt->length, madt = reinterpret_cast<acpi_madt_t*>(entry)) {
        switch (madt->type) {
            case 0:
                lapic.push_back(*reinterpret_cast<acpi_madt_lapic_t*>(entry));
                break;
            case 1:
                ioapic.push_back(*reinterpret_cast<acpi_madt_ioapic_t*>(entry));
                break;
            case 2:
                iso.push_back(*reinterpret_cast<acpi_madt_iso_t*>(entry));
                break;
            case 3:
                ionmi.push_back(*reinterpret_cast<acpi_madt_ionmi_t*>(entry));
                break;
            case 4:
                lnmi.push_back(*reinterpret_cast<acpi_madt_lnmi_t*>(entry));
                break;
            case 5:
                lacpicao.push_back(
                    *reinterpret_cast<acpi_madt_lapicao_t*>(entry));
                break;
            case 6:
                x2apic.push_back(
                    *reinterpret_cast<acpi_madt_lx2apic_t*>(entry));
                break;
        }
    }
}

void ec_init() {
    LAI_CLEANUP_STATE lai_state_t state;
    lai_init_state(&state);

    LAI_CLEANUP_VAR lai_variable_t pnp_id = LAI_VAR_INITIALIZER;
    lai_eisaid(&pnp_id, ACPI_EC_PNP_ID);

    lai_ns_iterator it = LAI_NS_ITERATOR_INITIALIZER;
    lai_nsnode_t* node;

    while ((node = lai_ns_iterate(&it))) {
        if (lai_check_device_pnp_id(node, &pnp_id, &state)) {
            continue;
        }

        auto driver = kernel::memory::physical::alloc<lai_ec_driver*>(
            kernel::memory::div_roundup(sizeof(lai_ec_driver),
                                        kernel::memory::physical::PAGE_SIZE));
        lai_init_ec(node, driver);

        lai_ns_child_iterator child_it =
            LAI_NS_CHILD_ITERATOR_INITIALIZER(node);
        lai_nsnode_t* child_node = nullptr;

        while ((child_node = lai_ns_child_iterate(&child_it))) {
            if (lai_ns_get_node_type(child_node) == LAI_NODETYPE_OPREGION) {
                lai_ns_override_opregion(child_node, &lai_ec_opregion_override,
                                         driver);
            }
        }
    }
}

void power_off() {
    lai_enter_sleep(5);
}

void init() {
    rsdp = reinterpret_cast<acpi_rsdp*>(
        memory::to_higher_half(rsdp_request.response->address));

    if (rsdp->revision >= 2 && rsdp->xsdt_address) {
        xsdt = true;
        rsdt = reinterpret_cast<acpi_rsdt*>(
            memory::to_higher_half(rsdp->xsdt_address));

        log::debug << "\nFound xsdt at " << rsdt << "\n";
    } else {
        xsdt = false;
        rsdt = reinterpret_cast<acpi_rsdt*>(
            memory::to_higher_half(rsdp->rsdt_address));

        log::debug << "\nFound rsdt at " << rsdt << "\n";
    }

    size_t entries =
        (rsdt->header.length - sizeof(acpi_sdt_header_t)) / (xsdt ? 8 : 4);

    if (entries != 0) {
        for (size_t i = 0; i < entries; i++) {
            auto header =
                reinterpret_cast<acpi_sdt_header_t*>(memory::to_higher_half(
                    xsdt ? rsdt->tablesx[i] : rsdt->tables[i]));

            if (header == nullptr) {
                continue;
            }

            auto checksum = [header]() -> bool {
                uint8_t sum = 0;

                for (size_t i = 0; i < header->length; i++) {
                    sum += reinterpret_cast<uint8_t*>(header)[i];
                }

                return sum == 0;
            };

            if (checksum()) {
                tables.push_back(header);
            }
        }
    }

    madt_header = reinterpret_cast<acpi_madt_header_t*>(find_table("APIC", 0));
    fadt_header = reinterpret_cast<acpi_fadt_t*>(find_table("FACP", 0));

    madt_init();

    lai_set_acpi_revision(rsdp->revision);
    lai_create_namespace();

    log::info << "\nInitialized ACPI!\n";
}

void enable(acpi_interrupt_model model) {
    ec_init();
    lai_enable_acpi(static_cast<uint32_t>(model));
}

void disable() {
    lai_disable_acpi();
}
}  // namespace kernel::drivers::acpi