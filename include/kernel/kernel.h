#pragma once

#include <limine.h>

namespace kernel {
extern limine_framebuffer_request framebuffer_request;
extern limine_memmap_request memmap_request;
extern limine_hhdm_request hhdm_request;
extern limine_paging_mode_request paging_mode_request;
extern limine_smp_request smp_request;
extern limine_kernel_file_request kernel_file_request;
extern limine_kernel_address_request kernel_address_request;
extern limine_rsdp_request rsdp_request;
extern limine_efi_system_table_request efi_system_table_request;

extern bool uefi;

extern "C" void _start();
}  // namespace kernel