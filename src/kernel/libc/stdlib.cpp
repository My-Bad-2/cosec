#include <stddef.h>
#include <stdlib.h>

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