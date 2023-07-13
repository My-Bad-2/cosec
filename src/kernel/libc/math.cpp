#include <math.h>

// contains a bunch of __builtin_ functions because I am too stupid
// to an actual implementations

double fabs(double arg) {
    return (arg >= 0) ? arg : -arg;
}

float fabsf(float arg) {
    return (arg >= 0) ? arg : -arg;
}

long double fabsl(long double arg) {
    return (arg >= 0) ? arg : -arg;
}

double fmod(double x, double y) {
    return x - trunc(x / y) * y;
}

float fmodf(float x, float y) {
    return x - truncf(x / y) * y;
}

long double fmodl(long double x, long double y) {
    return x - truncl(x / y) * y;
}

double remainder(double x, double y) {
    return __builtin_remainder(x, y);
}

float remainderf(float x, float y) {
    return __builtin_remainderf(x, y);
}

long double remainderl(long double x, long double y) {
    return __builtin_remainderl(x, y);
}

double exp(double x) {
    return __builtin_exp(x);
}

float expf(float x) {
    return __builtin_expf(x);
}

long double expl(long double x) {
    return __builtin_expl(x);
}

double exp2(double x) {
    return __builtin_exp2(x);
}

float exp2f(float x) {
    return __builtin_exp2f(x);
}

long double exp2l(long double x) {
    return __builtin_exp2l(x);
}

double expm1(double x) {
    return exp(x) - 1;
}

float expm1f(float x) {
    return expf(x) - 1;
}

long double expm1l(long double x) {
    return expl(x) - 1;
}

double log(double x) {
    return __builtin_log(x);
}

float logf(float x) {
    return __builtin_logf(x);
}

long double logl(long double x) {
    return __builtin_logl(x);
}

double log10(double x) {
    return __builtin_log10(x);
}

float log10f(float x) {
    return __builtin_log10f(x);
}

long double log10l(long double x) {
    return __builtin_log10l(x);
}

double log2(double x) {
    return __builtin_log2(x);
}

float log2f(float x) {
    return __builtin_log2f(x);
}

long double log2l(long double x) {
    return __builtin_log2l(x);
}

double log1p(double x) {
    return log(x + 1);
}

float log1pf(float x) {
    return log(x + 1);
}

long double log1pl(long double x) {
    return log(x + 1);
}

double pow(double base, double exponent) {
    return __builtin_pow(base, exponent);
}

float powf(float base, float exponent) {
    return __builtin_powf(base, exponent);
}

long double powl(long double base, long double exponents) {
    return __builtin_powl(base, exponents);
}

double sqrt(double x) {
    return __builtin_sqrt(x);
}

float sqrtf(float x) {
    return __builtin_sqrtf(x);
}

long double sqrtl(long double x) {
    return __builtin_sqrtl(x);
}

double cbrt(double x) {
    return __builtin_cbrt(x);
}

float cbrtf(float x) {
    return __builtin_cbrtf(x);
}

long double cbrtl(long double x) {
    return __builtin_cbrtl(x);
}

double hypot(double x, double y) {
    return sqrt(pow(x, 2) + pow(y, 2));
}

float hypotf(float x, float y) {
    return sqrtf(powf(x, 2) + powf(y, 2));
}

long double hypotl(long double x, long double y) {
    return sqrtl(powl(x, 2) + powl(y, 2));
}

double sin(double x) {
    return __builtin_sin(x);
}

float sinf(float x) {
    return __builtin_sinf(x);
}

long double sinl(long double x) {
    return __builtin_sinl(x);
}

double cos(double x) {
    return __builtin_cos(x);
}

float cosf(float x) {
    return __builtin_cosf(x);
}

long double cosl(long double x) {
    return __builtin_cosl(x);
}

double tan(double x) {
    return __builtin_tan(x);
}

float tanf(float x) {
    return __builtin_tanf(x);
}

long double tanl(long double x) {
    return __builtin_tanl(x);
}

double asin(double x) {
    return __builtin_asin(x);
}

float asinf(float x) {
    return __builtin_asinf(x);
}

long double asinl(long double x) {
    return __builtin_asinl(x);
}

double acos(double x) {
    return __builtin_acos(x);
}

float acosf(float x) {
    return __builtin_acosf(x);
}

long double acosl(long double x) {
    return __builtin_acosl(x);
}

double atan(double x) {
    return __builtin_atan(x);
}

float atanf(float x) {
    return __builtin_atanf(x);
}

double atan2(double x, double y) {
    return __builtin_atan2(x, y);
}

float atan2f(float x, float y) {
    return __builtin_atan2f(x, y);
}

long double atan2l(long double x, long double y) {
    return __builtin_atan2l(x, y);
}

double ceil(double x) {
    return __builtin_ceil(x);
}

float ceilf(float x) {
    return __builtin_ceilf(x);
}

long double ceill(long double x) {
    return __builtin_ceill(x);
}

double floor(double x) {
    return __builtin_floor(x);
}

float floorf(float x) {
    return __builtin_floorf(x);
}

long double floorl(long double x) {
    return __builtin_floorl(x);
}

double trunc(double x) {
    return __builtin_trunc(x);
}

float truncf(float x) {
    return __builtin_truncf(x);
}

long double truncl(long double x) {
    return __builtin_truncl(x);
}