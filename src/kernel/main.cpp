#include <limine.h>

#include <kernel.h>
#include <drivers/drivers.hpp>
#include <system/system.hpp>

namespace kernel {
limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0,
    .response = nullptr,
};

extern "C" void _start() {
    system::init();
    drivers::init();

    drivers::fb::plot_line({400, 300}, {100, 200}, 0XFFC0CB);
    asm volatile("hlt");
}
}  // namespace kernel