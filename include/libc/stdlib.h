#ifndef _STDLIB_H
#define _STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

// General utilities
typedef struct {
    int quot, rem;
} div_t;

typedef struct {
    long quot, rem;
} ldiv_t;

typedef struct {
    long long quot, rem;
} lldiv_t;

// Numeric conversion functions
double atof(const char* string);
int atoi(const char* string);
long atol(const char* string);
long long atoll(const char* string);

double strtod(const char* __restrict string, char** __restrict end);
float strtof(const char* __restrict string, char** __restrict end);
long double strtold(const char* __restrict string, char** __restrict end);
long strtol(const char* __restrict string, char** __restrict end, int base);
long long strtoll(const char* __restrict string, char** __restrict end, int base);
unsigned long strtoul(const char* __restrict string, char** __restrict end, int base);
unsigned long long strtoull(const char* __restrict string, char** __restrict end, int base);

// String Conversion functions
void itoa(char* buf, unsigned long int n, int base);

// Memory Management functions
void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void* calloc(size_t count, size_t size);

// Integer arithmetic functions
int abs(int number);
long labs(long number);
long long llabs(long long number);

div_t div(int number, int denom);
ldiv_t ldiv(long number, long denom);
lldiv_t lldiv(long long number, long long denom);

#ifdef __cplusplus
}
#endif

#endif