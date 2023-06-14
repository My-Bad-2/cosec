#pragma once

#include <kernel.h>
#include <stddef.h>
#include <stdint.h>

namespace kernel::memory {
void init();

extern uintptr_t hhdm_offset;

constexpr inline auto align_down(auto n, auto a) {
    return (n & ~(a - 1));
}

auto align_up(auto n, auto a) {
    return align_down(n + a - 1, a);
}

template <typename T>
constexpr inline bool is_higher_half(T a) {
    return (uintptr_t)a >= hhdm_offset;
}

template <typename T>
constexpr inline T from_higher_half(T a) {
    return !is_higher_half(a) ? a : (T)((uintptr_t)a - hhdm_offset);
}

template <typename T>
constexpr inline T to_higher_half(T a) {
    return is_higher_half(a) ? a : (T)((uintptr_t)a + hhdm_offset);
}

constexpr inline auto div_roundup(auto n, auto a) {
    return align_down(n + a - 1, a) / a;
}

}  // namespace kernel::memory