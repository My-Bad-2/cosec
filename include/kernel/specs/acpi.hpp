#pragma once

#include <stdint.h>
#include <acpispec/tables.h>

struct [[gnu::packed]] acpi_rsdp {
    uint8_t signature[8];
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
};

struct [[gnu::packed]] acpi_sdt_header_t {
    uint8_t signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    uint8_t oemid[6];
    uint8_t oem_table_id[8];
    uint32_t oem_revision;
    uint32_t create_id;
    uint32_t create_revision;
};

struct [[gnu::packed]] acpi_rsdt {
    acpi_sdt_header_t header;

    union {
        uint32_t tables[];
        uint64_t tablesx[];
    };
};

struct [[gnu::packed]] acpi_mcfg_entry_t {
    uint64_t base_address;
    uint16_t segment;
    uint8_t start_bus;
    uint8_t end_bus;
    uint32_t reserved;
};

struct [[gnu::packed]] acpi_mcfg_header_t {
    acpi_sdt_header_t header;
    uint64_t reserved;
    acpi_mcfg_entry_t entries[];
};

struct [[gnu::packed]] acpi_madt_header_t {
    acpi_sdt_header_t sdt;
    uint32_t lapic_address;
    uint32_t flags;
    char entries[];

    [[gnu::always_inline]] bool legacy_pic() { return this->flags & 0x01; }
};

struct [[gnu::packed]] acpi_madt_t {
    uint8_t type;
    uint8_t length;
};

struct [[gnu::packed]] acpi_madt_lapic_t {
    acpi_madt_t header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
};

struct [[gnu::packed]] acpi_madt_ioapic_t {
    acpi_madt_t header;
    uint8_t apic_id;
    uint8_t reserved;
    uint32_t address;
    uint32_t gsib;
};

struct [[gnu::packed]] acpi_madt_iso_t {
    acpi_madt_t header;
    uint8_t bus_source;
    uint8_t irq_source;
    uint32_t gsi;
    uint16_t flags;
};

struct [[gnu::packed]] acpi_madt_ionmi_t {
    acpi_madt_t header;
    uint8_t source;
    uint8_t reserved;
    uint16_t flags;
    uint32_t gsi;
};

struct [[gnu::packed]] acpi_madt_lnmi_t {
    acpi_madt_t header;
    uint8_t processor;
    uint16_t flags;
    uint8_t lint;
};

struct [[gnu::packed]] acpi_madt_lapicao_t {
    acpi_madt_t header;
    uint16_t reserved;
    uint64_t lapic_address;
};

struct [[gnu::packed]] acpi_madt_lx2apic_t {
    acpi_madt_t header;
    uint16_t reserved;
    uint32_t x2apic_id;
    uint32_t flags;
    uint32_t acpi_id;
};

struct [[gnu::packed]] acpi_generic_address_t {
    uint8_t address_space;
    uint8_t bit_width;
    uint8_t bit_offset;
    uint8_t access_size;
    uint64_t address;
};

struct [[gnu::packed]] acpi_hpet_header_t {
    acpi_sdt_header_t header;
    uint8_t hardware_rev_id;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t reserved : 1;
    uint8_t legacy_replacement : 1;
    uint16_t pci_vendor_id;
    acpi_generic_address_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
};

// struct [[gnu::packed]] acpi_fadt_header_t {
//     acpi_sdt_header_t header;

//     uint32_t firmware_contrl;
//     uint32_t dsdt;

//     uint8_t reserved;
//     uint8_t preferred_power_management_profile;

//     uint16_t sci_interrupt;
//     uint32_t smi_command_port;

//     uint8_t acpi_enable;
//     uint8_t acpi_disable;

//     uint8_t s4_bios_req;
//     uint8_t pstate_control;

//     uint32_t pm1a_event_block;
//     uint32_t pm1b_event_block;
//     uint32_t pm1a_control_block;
//     uint32_t pm1b_control_block;
//     uint32_t pm2_control_block;
//     uint32_t pm_timer_block;

//     uint32_t gpe0_block;
//     uint32_t gpe1_block;

//     uint8_t pm1_event_length;
//     uint8_t pm1_control_length;
//     uint8_t pm2_control_length;
//     uint8_t pm_timer_length;

//     uint8_t gpe0_length;
//     uint8_t gpe1_length;
//     uint8_t gpe1_base;

//     uint8_t c_state_control;
//     uint16_t worst_c2_latency;
//     uint16_t worst_c3_latency;

//     uint16_t flush_size;
//     uint16_t flush_stride;

//     uint8_t duty_offset;
//     uint8_t duty_width;

//     uint8_t day_alarm;
//     uint8_t month_alarm;
//     uint8_t century;

//     uint16_t boot_architecture_flags;
//     uint8_t reserved2;
//     uint32_t flags;

//     acpi_generic_address_t reset_reg;
//     uint8_t reset_value;

//     uint16_t arm_boot_architecture_flags;
//     uint8_t minor_version;

//     uint64_t xfirmware_control;
//     uint64_t xdsdt;

//     acpi_generic_address_t xpm1a_event_block;
//     acpi_generic_address_t xpm1b_event_block;
//     acpi_generic_address_t xpm1a_control_block;
//     acpi_generic_address_t xpm1b_control_block;
//     acpi_generic_address_t xpm2_control_block;
//     acpi_generic_address_t xpm_timer_block;
    
//     acpi_generic_address_t xgpe0_block;
//     acpi_generic_address_t xgpe1_block;
// };

struct [[gnu::packed]] dmar_header {
    acpi_sdt_header_t header;
    uint8_t host_address;
    uint8_t flags;
    uint8_t reserved[10];
    uint8_t remapping_structs[];
};