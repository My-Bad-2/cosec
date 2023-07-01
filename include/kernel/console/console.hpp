#pragma once

#include <cpp/term.hpp>
#include <stdint.h>
#include <string.h>
#include <common/math.h>
#include <vector>

namespace kernel::console {
struct terminal_t : cppterm_t {
    private:
    vec2<uint64_t> pos;

    public:
    static void callback(term_t* term, uint64_t type, uint64_t first, uint64_t second, uint64_t third);
};

extern std::vector<terminal_t*> terms;
extern terminal_t* main_term;
extern size_t term_count;
extern bool initialized;

void init();
void write(const char* buffer, terminal_t* term = main_term);
void printc(uint8_t c, terminal_t* term = main_term);
}