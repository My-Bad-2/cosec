#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>
#include <type_traits>

namespace std {
enum class align_val_t : std::size_t {};
}

static constexpr uint64_t ATEXIT_MAX_FUNCS = 128;

extern "C" {
using uarch_t = unsigned;

struct atexit_func_entry_t {
    void (*destructor_func)(void*);
    void* obj_ptr;
    void* dso_handle;
};

atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
uarch_t __atexit_func_count = 0;

void* __dso_handle = 0;

int __cxa_atexit(void (*func)(void*), void* objptr, void* dso) {
    if (__atexit_func_count >= ATEXIT_MAX_FUNCS)
        return -1;

    __atexit_funcs[__atexit_func_count].destructor_func = func;
    __atexit_funcs[__atexit_func_count].obj_ptr = objptr;
    __atexit_funcs[__atexit_func_count].dso_handle = dso;
    __atexit_func_count++;

    return 0;
};

void __cxa_finalize(void* func) {
    uarch_t i = __atexit_func_count;
    if (func == nullptr) {
        while (i--) {
            if (__atexit_funcs[i].destructor_func) {
                (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            }
        }
        return;
    }

    while (i--) {
        if (__atexit_funcs[i].destructor_func == func) {
            (*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
            __atexit_funcs[i].destructor_func = 0;
        };
    };
};
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
