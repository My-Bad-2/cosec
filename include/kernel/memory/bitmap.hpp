#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct bitmap_t {
    size_t size;
    uint8_t* buffer;

    constexpr bitmap_t() : size(0), buffer(nullptr) {}

    constexpr bitmap_t(uint8_t* buffer, size_t size)
        : size(size), buffer(buffer) {}

    constexpr bool set(size_t idx, bool value) {
        bool ret = this->get(idx);

        if (value) {
            this->buffer[idx / 8] |= (1 << (idx % 8));
        } else {
            this->buffer[idx / 8] &= ~(1 << (idx % 8));
        }

        return ret;
    }

    constexpr bool get(size_t idx) {
        return this->buffer[idx / 8] & (1 << (idx % 8));
    }

    constexpr bool operator[](size_t idx) { return this->get(idx); }
};