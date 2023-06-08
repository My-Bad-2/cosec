#include <drivers/drivers.hpp>
#include <kernel.h>

namespace kernel::drivers {
void init() {
    fb::init(framebuffer_request.response);
}
}