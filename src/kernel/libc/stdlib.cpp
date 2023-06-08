#include <stdlib.h>

template<typename T>
T _abs(T number) {
    return number < 0 ? -number : number;
}

int abs(int number) {
    return _abs(number);
}

long labs(long number) {
    return _abs(number);
}

long long llabs(long long number) {
    return _abs(number);
}