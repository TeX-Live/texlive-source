// Common/Types.h

#ifndef __COMMON_TYPES_H
#define __COMMON_TYPES_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <inttypes.h>

typedef uint8_t Byte;
typedef int32_t Int32;
typedef uint32_t UInt32;
typedef int64_t Int64;
typedef uint64_t UInt64;

// This looks wrong but earlier we used int_fast16_t and uint_fast16_t
// which were 32-bit or 64-bit on many systems. 32-bit integers give
// better speed on some systems with some compilers, so I don't like
// to change this to real 16-bit integer in LZMA Utils 4.32.x.
//
// 64-bit integers pollute the CPU's cache too badly, so it's better
// to avoid them. Even 32-bit integers aren't good for cache with
// large lc/lp values, but I still stick to 32-bit integers in 4.32.x
// since I try to avoid reports "performance got worse in 4.32.6".
//
// Some systems have problems with C headers in C++ mode (they don't
// give fast types) so using fixed-size integer type should help with
// that problem too.
typedef int32_t Int16;
typedef uint32_t UInt16;

#endif
