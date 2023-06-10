#include <system/system.hpp>
#include <drivers/serials.hpp>

namespace kernel::arch {
void init() {
    using namespace kernel::drivers;

    system::init();

    serial::init(serial::COM1);
}
}