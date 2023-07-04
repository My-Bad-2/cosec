#include <assert.h>
#include <arch.hpp>
#include <stdint.h>
#include <debug/log.hpp>

void assertion_failed(const char* msg, const char* file, int line) {
    disable_interrupts();

    log::error << "Kernel assertion failed (" << msg << ") - \n\tfile: " << file
               << ", line: " << line << "\n";

    while (true) {
        halt();
    }
}