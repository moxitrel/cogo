#ifndef NSTD_TYPE_H
#define NSTD_TYPE_H

// char[]
// sizeof
// alignas(), alignof(T)

// bool : _Bool
// false: 0
// true : 1
//
// noreturn: _Noreturn
//
// alignas(N|T) T x: _Alignas
// alignof(T)      : _Alignof
#include <stdalign.h>
#include <stdbool.h>
#include <stdnoreturn.h>

// int, size_t, intptr_t, intmax_t
//
// int8_t   uint8_t   int_fast8_t  uint_fast8_t
// int16_t  uint16_t  int_fast16_t uint_fast16_t
// int32_t  uint32_t  int_fast32_t uint_fast32_t
// int64_t  uint64_t  int_fast64_t uint_fast64_t
// intptr_t uintptr_t
// intmax_t uintmax_t
typedef unsigned int uint; // nat: natural number
#include <stdint.h>

// NULL
// offsetof(type, member-designator)
#include <stddef.h>

// CHAR_BIT
//  INT_MAX, INT_MIN
// UINT_MAX
#include <limits.h>

// NDEBUG
// static_assert: _Static_assert
#include <assert.h>

// errno : int, thread local storage duration
//
// EDOM  : Mathematics argument out of domain of function
// ERANGE: Result too large
// EILSEQ: Illegal byte sequence, a byte sequence does not form a valid
// wide-character code
//
// POSIX:
//   EINVAL: Invalid argument
//   ENOMEM: Not enough space
#include <errno.h>
#include <stdarg.h>

#endif // NSTD_TYPE_H
