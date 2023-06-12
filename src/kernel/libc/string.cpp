// Copyright 2015-2022 mlibc Contributors
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace {
template <typename T>
struct word_helper {
    using underlying [[gnu::aligned(1)]] = T;
    enum class [[gnu::may_alias]] word_enum : underlying {};
};

template <typename T>
using word = typename word_helper<T>::word_enum;

template <typename T>
[[gnu::always_inline]] inline word<T> alias_load(const uint8_t*& p) {
    word<T> value = *reinterpret_cast<const word<T>*>(p);
    p += sizeof(T);
    return value;
}

template <typename T>
[[gnu::always_inline]] inline void alias_store(uint8_t*& p, word<T> value) {
    *reinterpret_cast<word<T>*>(p) = value;
    p += sizeof(T);
}

#ifdef __LP64__
void* forward_copy(void* __restrict dest, const void* __restrict src,
                   size_t size) {
    auto cur_dest = reinterpret_cast<uint8_t*>(dest);
    auto cur_src = reinterpret_cast<const uint8_t*>(src);

    while (size >= 8 * 8) {
        auto w1 = alias_load<uint64_t>(cur_src);
        auto w2 = alias_load<uint64_t>(cur_src);
        auto w3 = alias_load<uint64_t>(cur_src);
        auto w4 = alias_load<uint64_t>(cur_src);
        auto w5 = alias_load<uint64_t>(cur_src);
        auto w6 = alias_load<uint64_t>(cur_src);
        auto w7 = alias_load<uint64_t>(cur_src);
        auto w8 = alias_load<uint64_t>(cur_src);

        alias_store<uint64_t>(cur_dest, w1);
        alias_store<uint64_t>(cur_dest, w2);
        alias_store<uint64_t>(cur_dest, w3);
        alias_store<uint64_t>(cur_dest, w4);
        alias_store<uint64_t>(cur_dest, w5);
        alias_store<uint64_t>(cur_dest, w6);
        alias_store<uint64_t>(cur_dest, w7);
        alias_store<uint64_t>(cur_dest, w8);

        size -= 8 * 8;
    }

    if (size >= 4 * 8) {
        auto w1 = alias_load<uint64_t>(cur_src);
        auto w2 = alias_load<uint64_t>(cur_src);
        auto w3 = alias_load<uint64_t>(cur_src);
        auto w4 = alias_load<uint64_t>(cur_src);

        alias_store<uint64_t>(cur_dest, w1);
        alias_store<uint64_t>(cur_dest, w2);
        alias_store<uint64_t>(cur_dest, w3);
        alias_store<uint64_t>(cur_dest, w4);

        size -= 4 * 8;
    }

    if (size >= 2 * 8) {
        auto w1 = alias_load<uint64_t>(cur_src);
        auto w2 = alias_load<uint64_t>(cur_src);

        alias_store<uint64_t>(cur_dest, w1);
        alias_store<uint64_t>(cur_dest, w2);

        size -= 2 * 8;
    }

    if (size >= 8) {
        auto w = alias_load<uint64_t>(cur_src);
        alias_store<uint64_t>(cur_dest, w);
        size -= 8;
    }

    if (size >= 4) {
        auto w = alias_load<uint32_t>(cur_src);
        alias_store<uint32_t>(cur_dest, w);
        size -= 4;
    }

    if (size >= 2) {
        auto w = alias_load<uint16_t>(cur_src);
        alias_store<uint16_t>(cur_dest, w);
        size -= 2;
    }

    if (size) {
        *cur_dest = *cur_src;
    }

    return dest;
}
#else
void* forward_copy(void* dest, const void* src, size_t size) {
    for (size_t i = 0; i < n; i++) {
        ((char*)dest)[i] = ((const char*)src)[i];
    }

    return dest;
}
#endif
}  // namespace

void* memcpy(void* __restrict dest, const void* __restrict src, size_t size) {
    return forward_copy(dest, src, size);
}

#ifdef __LP64__
void* memset(void* dest, int c, size_t size) {
    auto cur_dest = reinterpret_cast<uint8_t*>(dest);
    uint8_t byte = c;

    while (size && (reinterpret_cast<uintptr_t>(cur_dest) & 7)) {
        *cur_dest++ = byte;
        --size;
    }

    auto pattern64 = static_cast<word<uint64_t>>(
        static_cast<uint64_t>(byte) | (static_cast<uint64_t>(byte) << 8) |
        (static_cast<uint64_t>(byte) << 16) |
        (static_cast<uint64_t>(byte) << 24) |
        (static_cast<uint64_t>(byte) << 32) |
        (static_cast<uint64_t>(byte) << 40) |
        (static_cast<uint64_t>(byte) << 48) |
        (static_cast<uint64_t>(byte) << 56));

    auto pattern32 = static_cast<word<uint32_t>>(
        static_cast<uint32_t>(byte) | (static_cast<uint32_t>(byte) << 8) |
        (static_cast<uint32_t>(byte) << 16) |
        (static_cast<uint32_t>(byte) << 24));

    auto pattern16 = static_cast<word<uint16_t>>(
        static_cast<uint16_t>(byte) | (static_cast<uint16_t>(byte) << 8));

    while (size >= 8 * 8) {
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);

        size -= 8 * 8;
    }

    if (size >= 4 * 8) {
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);

        size -= 4 * 8;
    }

    if (size >= 2 * 8) {
        alias_store<uint64_t>(cur_dest, pattern64);
        alias_store<uint64_t>(cur_dest, pattern64);

        size -= 2 * 8;
    }

    if (size >= 8) {
        alias_store<uint64_t>(cur_dest, pattern64);
        size -= 8;
    }

    if (size >= 4) {
        alias_store<uint32_t>(cur_dest, pattern32);
        size -= 4;
    }

    if (size >= 2) {
        alias_store<uint16_t>(cur_dest, pattern16);
        size -= 2;
    }

    if (size) {
        *cur_dest = byte;
    }

    return dest;
}
#else
void* memset(void* dest, int c, size_t size) {
    for (size_t i = 0; i < count; i++) {
        ((char*)dest)[i] = (char)c;
    }

    return dest;
}
#endif

void* memmove(void* dest, const void* src, size_t size) {
    char* dest_bytes = (char*)dest;
    char* src_bytes = (char*)src;

    if (dest_bytes < src_bytes) {
        return forward_copy(dest, src, size);
    } else if (dest_bytes > src_bytes) {
        for (size_t i = 0; i < size; i++) {
            dest_bytes[size - i - 1] = src_bytes[size - i - 1];
        }
    }

    return dest;
}

size_t strlen(const char* s) {
    size_t len = 0;

    for (size_t i = 0; s[i]; i++) {
        len++;
    }

    return len;
}

char* strcpy(char* __restrict dest, const char* src) {
    char* dest_bytes = (char*)dest;
    char* src_bytes = (char*)src;

    while (*src_bytes) {
        *(dest_bytes++) = *(src_bytes++);
    }

    *dest_bytes = 0;

    return dest;
}

char* strncpy(char* __restrict dest, const char* src, size_t max_size) {
    auto dest_bytes = static_cast<char*>(dest);
    auto src_bytes = static_cast<const char*>(src);
    size_t i = 0;

    while (*src_bytes && i < max_size) {
        *(dest_bytes++) = *(src_bytes++);
        i++;
    }

    while (i < max_size) {
        *(dest_bytes++) = 0;
        i++;
    }

    return dest;
}

char* strcat(char* __restrict dest, const char* __restrict src) {
    strcpy(dest + strlen(dest), src);
    return dest;
}

char* strncat(char* __restrict dest, const char* __restrict src,
              size_t max_size) {
    auto dest_bytes = static_cast<char*>(dest);
    auto src_bytes = static_cast<const char*>(src);
    size_t i = 0;

    dest_bytes += strlen(dest);

    while (*src_bytes && i < max_size) {
        *(dest_bytes++) = *(src_bytes++);
        i++;
    }

    *dest_bytes = 0;
    return dest;
}

int memcmp(const void* a, const void* b, size_t size) {
    for (size_t i = 0; i < size; i++) {
        auto a_byte = static_cast<const uint8_t*>(a)[i];
        auto b_byte = static_cast<const uint8_t*>(b)[i];

        if (a_byte < b_byte) {
            return -1;
        }

        if (a_byte > b_byte) {
            return 1;
        }
    }

    return 0;
}

int strcmp(const char* a, const char* b) {
    size_t i = 0;

    while (true) {
        uint8_t a_byte = a[i];
        uint8_t b_byte = b[i];

        if (!a_byte && !b_byte) {
            return 0;
        }

        if (a_byte < b_byte) {
            return -1;
        }

        if (a_byte > b_byte) {
            return 1;
        }

        i++;
    }
}

int strncmp(const char* a, const char* b, size_t max_size) {
    size_t i = 0;

    while (true) {
        if (!(i < max_size)) {
            return 0;
        }

        uint8_t a_byte = a[i];
        uint8_t b_byte = b[i];

        if (!a_byte && !b_byte) {
            return 0;
        }

        if (a_byte < b_byte) {
            return -1;
        }

        if (a_byte > b_byte) {
            return 1;
        }

        i++;
    }
}

void* memchr(const void* s, int c, size_t size) {
    auto s_bytes = static_cast<const uint8_t*>(s);

    for (size_t i = 0; i < size; i++) {
        if (s_bytes[i] == static_cast<uint8_t>(c)) {
            return const_cast<uint8_t*>(s_bytes + i);
        }
    }

    return nullptr;
}

char* strchr(const char* s, int c) {
    size_t i = 0;

    while (s[i]) {
        if (s[i] == c) {
            return const_cast<char*>(&s[i]);
        }

        i++;
    }

    if (c == 0) {
        return const_cast<char*>(&s[i]);
    }

    return nullptr;
}