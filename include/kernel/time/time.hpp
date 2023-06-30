#pragma once

#include <stddef.h>
#include <stdint.h>

using time_t = long;
using seconds_t = long;

namespace kernel::time {
struct timespec {
    time_t tv_sec;
    time_t tv_nsec;

    constexpr timespec() : tv_sec(0), tv_nsec(0) {}

    constexpr timespec(time_t s, time_t ns) : tv_sec(s), tv_nsec(ns) {}

    constexpr timespec(time_t ms)
        : tv_sec(ms / 1000), tv_nsec((ms % 1000) * 1000000) {}

    constexpr time_t to_ns() {
        return this->tv_sec * 1000000000 + this->tv_nsec;
    }

    constexpr time_t to_ms() { return this->to_ns() / 1000000; }

    constexpr timespec& operator+=(const timespec& rhs) {
        this->tv_sec += rhs.tv_sec;
        this->tv_nsec += rhs.tv_nsec;

        while (this->tv_nsec >= 1000000000) {
            this->tv_sec++;
            this->tv_nsec -= 1000000000;
        }

        return *this;
    }

    friend constexpr timespec operator+(const timespec& lhs,
                                        const timespec& rhs) {
        timespec ret = lhs;
        return ret += rhs;
    }

    friend constexpr timespec operator+(const timespec& lhs, time_t ns) {
        return lhs + timespec(0, ns);
    }

    constexpr timespec& operator-=(const timespec& rhs) {
        this->tv_sec -= rhs.tv_sec;
        this->tv_nsec -= rhs.tv_nsec;

        while (this->tv_nsec < 0) {
            this->tv_sec--;
            this->tv_nsec -= 1000000000;
        }

        if (this->tv_sec < 0) {
            this->tv_sec = 0;
            this->tv_nsec = 0;
        }

        return *this;
    }

    friend constexpr timespec operator-(const timespec& lhs,
                                        const timespec& rhs) {
        timespec ret = lhs;
        return ret -= rhs;
    }

    friend constexpr timespec operator-(const timespec& lhs, time_t ns) {
        return lhs - timespec(0, ns);
    }
};

struct timeval {
    time_t tv_sec;
    seconds_t tv_usec;
};

extern timespec realtime;
extern timespec monotonic;

void timer_handler(size_t ns);

uint64_t time_ns();
uint64_t time_ms();

void msleep(uint64_t ms);
void nsleep(uint64_t ns);
}  // namespace kernel::time