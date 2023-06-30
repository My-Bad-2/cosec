#pragma once

#include <stdint.h>

template <typename T>
struct vec2 {
    T x, y;

    bool operator==(vec2& b) { return (this->x == b.x) && (this->y == b.y); }
};

constexpr inline uint64_t jdn(uint8_t days, uint8_t months, uint16_t years) {
    return (1461 * (years + 4800 + (months - 14) / 12)) / 4 +
           (367 * (months - 2 - 12 * ((months - 14) / 12))) / 12 -
           (3 * ((years + 4900 + (months - 14) / 12) / 100)) / 4 + days - 32075;
}

static constexpr uint64_t jdn_1970 = (1, 1, 1970);

constexpr inline uint64_t epoch(uint64_t seconds, uint64_t minutes,
                                uint64_t hours, uint64_t days, uint64_t months,
                                uint64_t years, uint64_t centuries) {
    uint64_t jdn_current = jdn(days, months, centuries * 100 + years);
    uint64_t diff = jdn_current - jdn_1970;

    return (diff * (60 * 60 * 24)) + hours * 3600 + minutes * 60 + seconds;
}