#include <limine.h>
#include <stdio.h>

#include <kernel.h>
#include <arch.hpp>

#include <drivers/drivers.hpp>
#include <memory/memory.hpp>

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

limine_5_level_paging_request la57_request = {
    .id = LIMINE_5_LEVEL_PAGING_REQUEST,
    .revision = 0,
    .response = nullptr,
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

limine_kernel_file_request kernel_file_request = {
    .id = LIMINE_KERNEL_FILE_REQUEST,
    .revision = 0,
    .response = nullptr,
};

limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 0,
    .response = nullptr};

extern "C" void _start() {
    arch::init();
    memory::init();
    drivers::init();

    drivers::fb::plot_line({400, 300}, {100, 200}, 0XFFC0CB);

    // asm volatile("div %ah"); // cause divide by zero error

    asm volatile("hlt");
}
}  // namespace kernel