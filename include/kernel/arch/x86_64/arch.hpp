#pragma once

#include <stddef.h>
#include <cstdint>
#include <frg/optional.hpp>

namespace kernel::arch {
void early_init();
void init();
uint64_t time_ns();
uint64_t epoch();
}

extern "C" {
size_t println(const char* str);
void printc(char c);
}