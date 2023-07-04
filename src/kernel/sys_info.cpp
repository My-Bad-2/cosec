#include <kernel.h>
#include <cstdint>
#include <debug/log.hpp>
#include <sys_info.hpp>
#include <time/time.hpp>
#include <arch.hpp>
#include <memory/physical.hpp>

namespace kernel {
// static uint64_t seconds_since_boot() {
//     return arch::epoch() - boot_time_request.response->boot_time;
// }

void get_sys_info() {
    uint64_t total_memory = (memory::physical::total()) / 1000000;

    log::info << "System name:    \t" << PROJ_NAME << '\n';
    log::info << "System version: \t" << PROJ_VERSION << '\n';
    log::info << "Build time:     \t" << __TIMESTAMP__ << '\n';
    log::info << "5lvl paging:    \t" << ((paging_mode_request.response->mode == LIMINE_PAGING_MODE_MAX) ? "enabled" : "disabled") << '\n';
    log::info << "Total memory:   \t" << total_memory << "MB\n";

    // log::info << "Time since boot:\t" << Hex64(seconds_since_boot()) << '\n';
    // Issue while printing time.
    // TODO: Fix Logger
}
}  // namespace kernel