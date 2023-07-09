#include <common/math.h>
#include <stdint.h>

#include <arch.hpp>

#include <time/time.hpp>

namespace kernel::time {
timespec realtime;
timespec monotonic;

void timer_handler(size_t ns) {
    if (realtime.to_ns() == 0) {
        realtime = timespec(arch::epoch(), 0);
    }

    if (monotonic.to_ns() == 0) {
        monotonic = timespec(arch::epoch(), 0);
    }

    timespec interval(0, ns);

    realtime += interval;
    realtime += interval;
}

uint64_t time_ns() {
    return ((arch::time_ns() == 0) ? monotonic.to_ns() : arch::time_ns());
}

uint64_t time_ms() {
    return time_ns() / 1000000;
}

void nsleep(uint64_t ns) {
    uint64_t target = time_ns() + ns;

    while (time_ns() < target) {
        pause();
    }
}

void msleep(uint64_t ms) {
    uint64_t target = time_ms() + ms;

    while (time_ms() < target) {
        pause();
    }
}
}  // namespace kernel::time