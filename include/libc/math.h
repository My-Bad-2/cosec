#ifndef _MATH_H
#define _MATH_H

#include <stddef.h>
#include <stdint.h>

#define HUGE_VAL __builtin_huge_val()
#define HUGE_VALF __builtin_huge_valf()
#define HUGE_VALL __builtin_huge_vall()

#define INFINITY __builtin_inff()
#define NAN __builtin_nanf("")

#define isfinite(x) __builtin_isfinite(x)
#define isinf(x) __builtin_isinf(x)
#define isnan(x) __builtin_isnan(x)
#define signbit(x) __builtin_signbit(x)

#define isgreater(x, y) (x > y)
#define isgraterequal(x, y) (x >= y)
#define isless(x, y) (x < y)
#define islessequal(x, y) (x <= y)
#define islessgreater (x < y) || (x > y)
#define isunordered(x, y) __builtin_isunordered(x, y)
#define isnormal(arg) __builtin_isnormal(x)

typedef float float_t;
typedef double double_t;

// Basic operations
double fabs(double arg);
float fabsf(float arg);
long double fabsl(long double arg);

double fmod(double x, double y);
float fmodf(float x, float y);
long double fmodl(long double x, long double y);

double remainder(double x, double y);
float remainderf(float x, float y);
long double remainderl(long double x, long double y);

// Exponential functions
double exp(double x);
float expf(float x);
long double expl(long double x);

double exp2(double x);
float exp2f(float x);
long double exp2l(long double x);

double expm1(double x);
float expm1f(float x);
long double expm1l(long double x);

double log(double x);
float logf(float x);
long double logl(long double x);

double log10(double x);
float log10f(float x);
long double log10l(long double x);

double log2(double x);
float log2f(float x);
long double log2l(long double x);

double log1p(double x);
float log1pf(float x);
long double log1pl(long double x);

// Power functions
double pow(double base, double exponent);
float powf(float base, float exponent);
long double powl(long double base, long double exponents);

double sqrt(double x);
float sqrtf(float x);
long double sqrtl(long double x);

double cbrt(double x);
float cbrtf(float x);
long double cbrtl(long double x);

double hypot(double x, double y);
float hypotf(float x, float y);
long double hypotl(long double x, long double y);

// trignometic functions
double sin(double x);
float sinf(float x);
long double sinl(long double x);

double cos(double x);
float cosf(float x);
long double cosl(long double x);

double tan(double x);
float tanf(float x);
long double tanl(long double x);

double asin(double x);
float asinf(float x);
long double asinl(long double x);

double acos(double x);
float acosf(float x);
long double acosl(long double x);

double atan(double x);
float atanf(float x);
long double atanl(long double x);

double atan2(double x, double y);
float atan2f(float x, float y);
long double atan2l(long double x, long double y);

// Nearest integer floating-point operations
double ceil(double x);
float ceilf(float x);
long double ceill(long double x);

double floorc(double x);
float floorf(float x);
long double floorl(long double x);

double trunc(double x);
float truncf(float x);
long double truncl(long double x);

#endif