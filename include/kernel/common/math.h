#pragma once

#include <stdint.h>

template <typename T>
struct vec2 {
    T x, y;

    bool operator==(vec2& b) {
        return (this->x == b.x) && (this->y == b.y);
    }
};