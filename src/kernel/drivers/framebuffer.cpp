#include <drivers/framebuffer.hpp>

#include <common/math.h>
#include <limine.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <debug/log.hpp>

namespace kernel::drivers::fb {
limine_framebuffer** framebuffers = nullptr;
limine_framebuffer* main_frm = nullptr;
uint64_t frm_count = 0;

Color::Color(uint32_t color) {
    this->r = ((color >> 16) & 0xFF);
    this->g = ((color >> 8) & 0xFF);
    this->b = (color & 0xFF);
}

void init(limine_framebuffer_response* framebuffer) {
    frm_count = framebuffer->framebuffer_count;
    framebuffers = framebuffer->framebuffers;
    main_frm = framebuffers[0];

    memset(main_frm->address, 0,
           main_frm->width * main_frm->height * sizeof(uint64_t));

    log::info << "Initialized Framebuffer!\n";
}

void plot_pixel(uint64_t x, uint64_t y, Color color, limine_framebuffer* frm) {
    uint32_t* addr = reinterpret_cast<uint32_t*>(
        reinterpret_cast<uint64_t>(frm->address) +
        ((y * (frm->pitch / (frm->bpp / 8)) * 4)) + (x * 4));

    uint32_t rgb = (color.r << 16) | (color.g << 8) | color.b;

    *addr = rgb;
}

namespace details {
void plot_line_low(vec2<uint64_t> p0, vec2<uint64_t> p1, Color color) {
    uint64_t dx = p1.x - p0.x;
    uint64_t dy = p1.y - p0.y;
    uint64_t yi = 1;

    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    uint64_t D = (2 * dy) - dx;
    uint64_t y = p0.y;

    for (uint64_t x = p0.x; x < p1.x; x++) {
        plot_pixel(x, y, color);

        if (D > 0) {
            y += yi;
            D += (x * (dy - dx));
        } else {
            D += 2 * dy;
        }
    }
}

void plot_line_high(vec2<uint64_t> p0, vec2<uint64_t> p1, Color color) {
    uint64_t dx = p1.x - p0.x;
    uint64_t dy = p1.y - p0.y;
    uint64_t xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    uint64_t D = (2 * dx) - dy;
    uint64_t x = p0.x;

    for (uint64_t y = p0.y; y < p1.y; y++) {
        plot_pixel(x, y, color);

        if (D > 0) {
            x += xi;
            D += (2 * (dx - dy));
        } else {
            D += 2 * dx;
        }
    }
}
}  // namespace details

void plot_line(vec2<uint64_t> p0, vec2<uint64_t> p1, Color color) {
    if (abs(p1.y - p0.y) < abs(p1.x - p0.x)) {
        if (p0.x > p1.x) {
            details::plot_line_low(p1, p0, color);
        } else {
            details::plot_line_low(p0, p1, color);
        }
    } else {
        if (p0.y > p1.y) {
            details::plot_line_high(p1, p0, color);
        } else {
            details::plot_line_high(p0, p1, color);
        }
    }
}
}  // namespace kernel::drivers::fb