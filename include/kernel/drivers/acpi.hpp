#pragma once

#include <stddef.h>
#include <stdint.h>
#include <specs/acpi.hpp>
#include <vector>

namespace kernel::drivers::acpi {
extern acpi_fadt_t* fadt_header;
extern acpi_madt_header_t* madt_header;

extern std::vector<acpi_madt_lapic_t> lapic;
extern std::vector<acpi_madt_ioapic_t> ioapic;
extern std::vector<acpi_madt_iso_t> iso;
extern std::vector<acpi_madt_ionmi_t> ionmi;
extern std::vector<acpi_madt_lnmi_t> lnmi;
extern std::vector<acpi_madt_lapicao_t> lacpicao;
extern std::vector<acpi_madt_lx2apic_t> x2apic;

extern acpi_rsdp* rsdp;
extern acpi_rsdt* rsdt;
extern bool xsdt;

enum class acpi_interrupt_model {
    PIC,
    APIC,
    SAPIC,
};

enum acpi_sleep_state {
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
};

void init();
void enable(acpi_interrupt_model model);
void disable();

void* find_table(const char* signature, int index = 0);

void reset();
}  // namespace kernel::drivers::acpi