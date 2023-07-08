#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef unsigned char u_char;
typedef unsigned int u_int;
typedef unsigned short ushort;
typedef unsigned short u_short;
typedef unsigned long u_long;
typedef unsigned long int u_quad_t;
typedef long int quad_t;

typedef unsigned long dev_t; // Device numbers
typedef unsigned long ino_t; // File serial numbers
typedef unsigned int gid_t; // group id
typedef long off_t; //  File sizes
typedef int pid_t; // process & process group id
typedef int tid_t; // thread id

typedef long ssize_t;

typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;

typedef unsigned int useconds_t;
typedef long suseconds_t;

typedef long clock_t;
typedef int clockid_t;
typedef long time_t;

#endif