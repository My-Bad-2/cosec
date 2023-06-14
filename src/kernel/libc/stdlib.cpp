#include <stddef.h>
#include <stdlib.h>
#include <memory/heap.hpp>

namespace {
template <typename T>
T _abs(T number) {
    return number < 0 ? -number : number;
}
}  // namespace

int abs(int number) {
    return _abs(number);
}

long labs(long number) {
    return _abs(number);
}

long long llabs(long long number) {
    return _abs(number);
}

div_t div(int number, int denom) {
    return {number / denom, number % denom};
}

ldiv_t ldiv(long number, long denom) {
    return {number / denom, number % denom};
}

lldiv_t lldiv(long long number, long long denom) {
    return {number / denom, number % denom};
}

void itoa(char* buf, unsigned long int n, int base) {
    unsigned long int tmp = n;
    unsigned i = 0, j;

    do {
        tmp = n % base;
        buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
    } while (n /= base);

    buf[i--] = 0;

    for (j = 0; j < i; j++, i--) {
        tmp = buf[i];
        buf[j] = buf[i];
        buf[i] = tmp;
    }
}

using namespace kernel::memory::heap;

void* malloc(size_t size) {
    return allocator->malloc(size);
}

void free(void* ptr) {
    return allocator->free(ptr);
}

void* realloc(void* ptr, size_t size) {
    return allocator->realloc(ptr, size);
}

void* calloc(size_t count, size_t size) {
    return allocator->calloc(count, size);
}