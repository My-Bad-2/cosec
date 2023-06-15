#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

void assertion_failed(const char* msg, const char* file, int line) __attribute__((noreturn));

#define assert(expr) (void)((expr) || (assertion_failed(#expr, __FILE__, __LINE__), 0))

#ifdef __cplusplus
}
#endif

#endif