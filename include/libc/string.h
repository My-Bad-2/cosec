#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>
#include <stdint.h>

#ifndef __cplusplus
extern "C" {
#endif

// Copying functions
void* memcpy(void* __restrict dest, const void* __restrict src, size_t size);
void* memmove(void* dest, const void* src, size_t size);
char* strcpy(char* __restrict dest, const char* src);
char* strncpy(char* __restrict dest, const char* src, size_t max_size);

// Concatenation functions
char* strcat(char* __restrict dest, const char* __restrict src);
char* strncat(char* __restrict dest, const char* __restrict src, size_t max_size);

// Comparison functions
int memcmp(const void* a, const void* b, size_t size);
int strcmp(const char* a, const char* b);
int strncmp(const char* a, const char* b, size_t max_size);

// Search functions
void* memchr(const void* s, int c, size_t size);
char* strchr(const char* s, int c);

// Miscellaneous functions
void* memset(void* dest, int c, size_t size);
size_t strlen(const char* s);

#ifndef __cplusplus
}
#endif

#endif