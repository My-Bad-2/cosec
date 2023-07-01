#include <kernel.h>
#include <cstdint>
#include <debug/log.hpp>
#include <sys_info.hpp>
#include <time/time.hpp>

namespace kernel {
void get_sys_info() {

    log::info << "System name:   \t" << PROJ_NAME << '\n';
    log::info << "System version:\t" << PROJ_VERSION << '\n';
    log::info << "Build time:    \t" << __TIMESTAMP__ << '\n';
}
}  // namespace kernel