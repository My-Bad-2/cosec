#include <stdlib.h>
#include <cstddef>
#include <new>
#include <type_traits>

namespace std {
enum class align_val_t : std::size_t {};
}

void* operator new(size_t size) {
    return malloc(size);
}

void* operator new(size_t size, std::align_val_t) {
    return malloc(size);
}

void* operator new[](size_t size) {
    return malloc(size);
}

void* operator new[](size_t size, std::align_val_t) {
    return malloc(size);
}

void operator delete(void* ptr) noexcept {
    free(ptr);
}

void operator delete(void* ptr, std::align_val_t) noexcept {
    free(ptr);
}

void operator delete(void* ptr, size_t) noexcept {
    free(ptr);
}

void operator delete[](void* ptr) noexcept {
    free(ptr);
}

void operator delete[](void* ptr, std::align_val_t) noexcept {
    free(ptr);
}

void operator delete[](void* ptr, size_t) {
    free(ptr);
}