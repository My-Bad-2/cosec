#pragma once
#include <stddef.h>

namespace kernel::arch {
void init();
}

extern "C" {
size_t println(const char* str);
void printc(char c);
}