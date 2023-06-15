#include <assert.h>
#include <cstdint>
#include <debug/log.hpp>

void assertion_failed(const char* msg, const char* file, int line) {
    asm volatile("cli");

    log::error << "Kernel assertion failed (" << msg << ") - \n\tfile: " << file
               << ", line: " << line << "\n";

    while (true) {
        asm volatile("hlt");
    }
}