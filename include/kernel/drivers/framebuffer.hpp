#pragma once

#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include "common/math.h"

namespace kernel::drivers::fb {
extern limine_framebuffer** framebuffers;
extern limine_framebuffer* main_frm;
extern uint64_t frm_count;

struct color_t {
    uint8_t r, g, b;

    color_t(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    color_t(uint32_t color);
};

void init(limine_framebuffer_response* framebuffer);
void plot_pixel(uint64_t x, uint64_t y, color_t color, limine_framebuffer* frm = main_frm);
void plot_line(vec2<uint64_t> p0, vec2<uint64_t> p1, color_t color);
}