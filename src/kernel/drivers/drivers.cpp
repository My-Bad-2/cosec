#include <kernel.h>
#include <drivers/acpi.hpp>
#include <drivers/drivers.hpp>
#include <drivers/pci.hpp>
#include <debug/log.hpp>

namespace kernel::drivers {
void init() {
    // acpi::init();
    fb::init(framebuffer_request.response);

    log::info << '\n';
    
    pci::enumerate();
    acpi::enable(acpi::acpi_interrupt_model::APIC);
    
    log::info << '\n';
}
}  // namespace kernel::drivers