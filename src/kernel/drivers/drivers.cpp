#include <kernel.h>
#include <drivers/acpi.hpp>
#include <drivers/drivers.hpp>

namespace kernel::drivers {
void init() {
    acpi::init();
    fb::init(framebuffer_request.response);

    // acpi::enable(acpi::acpi_interrupt_model::APIC);
}
}  // namespace kernel::drivers