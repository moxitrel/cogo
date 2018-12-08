#ifndef NSTD_H
#define NSTD_H

#include "freestanding.h"

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

typedef unsigned int nat;   // natural number
typedef unsigned int uint;

#endif //NSTD_H
