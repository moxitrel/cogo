/*
 * void *NEW    (size_t): C++ ::operator new()   , allocate memory of size n
 * void  DELETE (T *)   : C++ ::operator delete(), release  memory
 *
 * */
#ifndef NSTD_MEMORY_H
#define NSTD_MEMORY_H

#include "type.h"
#include <stdio.h>
#include <stdlib.h>

//#define ASSERT_MSG(E, ...)                                                                  \
//(                                                                                           \
//    (E) ? (void)0                                                                           \
//        : ( printf("[%s %s %u] %s: %s\n", __FILE__, __func__, __LINE__, #E, #__VA_ARGS__)   \
//          , abort()                                                                         \
//          )                                                                                 \
//)

// alignas(N|T) char (*p)[N];   // object of compatible type
//
#define NEW(N) malloc_assert((N), __FILE__, __func__, __LINE__)
inline static void *malloc_assert(size_t n, const char *file, const char *func, int line)
{
    void *v = malloc(n);
    if (!v) {
        printf("[%s %s %u] ENOMEM: malloc(%zd)\n", file, func, line, n);
        exit(EXIT_FAILURE);
    }
    return v;
}

#define CALLOC(N, M) calloc_assert((N), (M), __FILE__, __func__, __LINE__)
inline static void *calloc_assert(size_t n, size_t m, const char *file, const char *func, int line)
{
    void *v = calloc(n, m);
    if (!v) {
        printf("[%s %s %u] ENOMEM: calloc(%zd)\n", file, func, line, n);
        exit(EXIT_FAILURE);
    }
    return v;
}

// free(NULL): "nop"
//
// void DELETE(T *): release memory, =C++ operator delete()
#define DELETE(P) ((void)(free(P), (P) = NULL))

// T: Pointer Type
#define SIZE_OF(T) (sizeof *(T){0})
#define ZERO_OF(T) ((T) & (char[SIZE_OF(T)]){})

#endif // NSTD_MEMORY_H
