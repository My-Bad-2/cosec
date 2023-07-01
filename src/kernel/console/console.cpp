#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <kernel.h>
#include <term.h>
#include <unifont.h>

#include <console/console.hpp>
#include <debug/log.hpp>
#include <drivers/framebuffer.hpp>

namespace kernel::console {
using namespace drivers::fb;

std::vector<terminal_t*> terms;
terminal_t* main_term = nullptr;
size_t term_count = 0;

bool initialized = false;

void terminal_t::callback(term_t* term, uint64_t type, uint64_t first,
                          uint64_t second, uint64_t third) {
    terminal_t* _term = reinterpret_cast<terminal_t*>(term);

    switch (type) {
        case TERM_CB_DEC:
            break;
        case TERM_CB_BELL:
            break;
        case TERM_CB_PRIVATE_ID:
            break;
        case TERM_CB_STATUS_REPORT:
            break;
        case TERM_CB_POS_REPORT:
            _term->pos.x = first;
            _term->pos.y = second;
            break;
        case TERM_CB_KBD_LEDS:
            break;
        case TERM_CB_MODE:
            break;
        case TERM_CB_LINUX:
            break;
    }
}

void init() {
#ifdef __x86_64__
    if (frm_count == 0) {
        log::error << "Couldn't get a framebuffer\n";
        assert(uefi != true);

        terminal_t* term = new terminal_t;

        term->init(term->callback, true);
        term->textmode();

        terms.push_back(term);
        main_term = term;

        return;
    }
#endif

    font_t font = {
        reinterpret_cast<uintptr_t>(&unifont),
        UNIFONT_WIDTH,
        UNIFONT_HEIGHT,
        1,
        1,
        1,
    };

    style_t style = {
        DEFAULT_ANSI_COLOURS, DEFAULT_ANSI_BRIGHT_COLOURS,
        DEFAULT_BACKGROUND,   DEFAULT_FOREGROUND,
        DEFAULT_MARGIN,       DEFAULT_MARGIN_GRADIENT,
    };

    background_t back = {
        NULL,
        STRETCHED,
        DEFAULT_BACKGROUND,
    };

    for (size_t i = 0; i < frm_count; i++) {
        terminal_t* term = new terminal_t;

        framebuffer_t frm{
            reinterpret_cast<uintptr_t>(framebuffers[i]->address),
            framebuffers[i]->width,
            framebuffers[i]->height,
            framebuffers[i]->pitch,
        };

        term->init(term->callback, uefi == false, 4);
        term->vbe(frm, font, style, back);

        terms.push_back(term);

        if (main_term == nullptr) {
            main_term = term;
        }

        term->disable_cursor();
    }

    term_count = terms.size();
    initialized = true;
}

void printc(uint8_t c, terminal_t* term) {
    term->putchar(c);
}

void write(const char* buffer, terminal_t* term) {
    term->write(buffer, strlen(buffer));
}
}  // namespace kernel::console

extern "C" {
void* alloc_mem(size_t size) {
    return malloc(size);
}

void free_mem(void* ptr, size_t size) {
    (void)size;
    free(ptr);
}
}