#include <kernel.h>
#include <drivers/drivers.hpp>

namespace kernel::drivers {
void init() {
    fb::init(framebuffer_request.response);
}
}  // namespace kernel::drivers