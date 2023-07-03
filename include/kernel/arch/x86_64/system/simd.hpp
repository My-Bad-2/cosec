#pragma once

#include <stdint.h>
#include <stddef.h>

namespace system::simd {
void init();

size_t ctx_size();
void ctx_init(void* ptr);
void ctx_save(void* ptr);
void ctx_load(void* ptr);
}