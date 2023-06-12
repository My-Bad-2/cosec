#include <limine.h>
#include <stdio.h>

#include <kernel.h>
#include <arch.hpp>

#include <drivers/drivers.hpp>

namespace kernel {
limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr,
};

extern "C" void _start() {
    arch::init();
    drivers::init();

    drivers::fb::plot_line({400, 300}, {100, 200}, 0XFFC0CB);

    // asm volatile("div %ah"); // cause divide by zero error

    asm volatile("hlt");
}
}  // namespace kernel