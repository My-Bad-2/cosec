#include <kernel.h>
#include <drivers/acpi.hpp>
#include <drivers/drivers.hpp>
#include <drivers/pci.hpp>

namespace kernel::drivers {
void init() {
    acpi::init();
    fb::init(framebuffer_request.response);

    pci::enumerate();
    acpi::enable(acpi::acpi_interrupt_model::APIC);
}
}  // namespace kernel::drivers