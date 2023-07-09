#include <kernel.h>
#include <limine.h>
#include <stdio.h>
#include <stdlib.h>

#include <arch.hpp>

#include <console/console.hpp>
#include <debug/log.hpp>
#include <drivers/acpi.hpp>
#include <drivers/drivers.hpp>
#include <memory/memory.hpp>
#include <sys_info.hpp>

namespace kernel {
limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_paging_mode_request paging_mode_request = {
    .id = LIMINE_PAGING_MODE_REQUEST,
    .revision = 0,
    .response = nullptr,
    .mode = LIMINE_PAGING_MODE_MAX,
    .flags = 0,
};

limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0,
    .response = nullptr,
#ifdef __x86_64__
    .flags = LIMINE_SMP_X2APIC,
#else
    .flags = 0,
#endif
};

limine_rsdp_request rsdp_request = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_kernel_file_request kernel_file_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_efi_system_table_request efi_system_table_request = {
    .id = LIMINE_EFI_SYSTEM_TABLE_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_boot_time_request boot_time_request = {
    .id = LIMINE_BOOT_TIME_REQUEST,
    .revision = 0,
    .response = nullptr,
};

bool uefi = false;

extern "C" void _start() {
    uefi = (efi_system_table_request.response != nullptr);

    arch::early_init();

    memory::init();
    drivers::acpi::init();

    arch::init();
    drivers::init();

    console::init();

    // drivers::fb::plot_line({400, 300}, {100, 200}, 0XFFC0CB);
    get_sys_info();

    // asm volatile("div %ah"); // cause divide by zero error

    halt();
}
}  // namespace kernel