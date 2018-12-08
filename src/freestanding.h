#ifndef FREESTANDING_H
#define FREESTANDING_H

//
// unsigned char[]
// sizeof
// alignas(), alignof(T)
//

// alignas(N|T) T x: _Alignas
// alignof(T)      : _Alignof
#include <stdalign.h>
// noreturn: _Noreturn
#include <stdnoreturn.h>
//#include <iso646.h>

// bool : _Bool
// false: 0
// true : 1
#include <stdbool.h>
// int, size_t, intptr_t, intmax_t
//
// int8_t   uint8_t   int_fast8_t  uint_fast8_t  int_least8_t  uint_least8_t
// int16_t  uint16_t  int_fast16_t uint_fast16_t int_least16_t uint_least16_t
// int32_t  uint32_t  int_fast32_t uint_fast32_t int_least32_t uint_least32_t
// int64_t  uint64_t  int_fast64_t uint_fast64_t int_least64_t uint_least64_t
// intptr_t uintptr_t
// intmax_t uintmax_t
#include <stdint.h>

// size_t, ptrdiff_t
// NULL
// offsetof(type, member-designator)
#include <stddef.h>

// CHAR_BIT
//  INT_MAX, INT_MIN
// UINT_MAX
#include <limits.h>
#include <float.h>

#include <stdarg.h>

#endif // FREESTANDING_H
