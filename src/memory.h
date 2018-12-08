/*
 * void *MALLOC (size_t)        : =malloc
 * void *CALLOC (size_t, size_t): =calloc
 * void  FREE   (T *)           : =free
 *
 * */
#ifndef NSTD_MEMORY_H
#define NSTD_MEMORY_H

#include "nstd.h"
#include <stdlib.h>
#include <stdio.h>

// FATAL(format, arg, ...)
#define FATAL(...)                                                  \
(   printf("[F %s %d %s()] ", __FILE__, __LINE__, __func__)         \
,   printf("" __VA_ARGS__)      /* append string literal */         \
,   printf("\n")                                                    \
,   exit(EXIT_FAILURE)          /* abort() */                       \
)


#ifndef NDEBUG
    // ASSERT(exp, format, arg, ...)
    #define ASSERT(E, ...)                                                      \
    (                                                                           \
        (E) ? (void)0                                                           \
            : ( printf("[A %s %d %s()] %s: ", __FILE__, __LINE__, __func__, #E) \
              , printf("" __VA_ARGS__)      /* append string literal */         \
              , printf("\n")                                                    \
              , exit(EXIT_FAILURE)          /* abort() */                       \
              )                                                                 \
    )
#else
    #define ASSERT(E, ...)  ((void)0)
#endif

// alignas(N|T) char (*p)[N];   // object of compatible type
//
#define MALLOC(N) malloc_assert((N), __FILE__, __func__, __LINE__, #N)
inline static void *malloc_assert(size_t n, const char *file, const char *func, int line, const char *arg)
{
    void *v = malloc(n);
    if (!v) {
        printf("[F %s %d %s()] MALLOC(%s): ENOMEM, malloc(%zu)\n", file, line, func, arg, n);
        exit(EXIT_FAILURE);
    }
    return v;
}

#define CALLOC(N, M) calloc_assert((N), (M), __FILE__, __func__, __LINE__, #N, #M)
inline static void *calloc_assert(size_t n, size_t m, const char *file, const char *func, int line, const char *arg1, const char *arg2)
{
    void *v = calloc(n, m);
    if (!v) {
        printf("[F %s %d %s()] CALLOC(%s, %s): ENOMEM, calloc(%zu, %zu)\n", file, line, func, arg1, arg2, n, m);
        exit(EXIT_FAILURE);
    }
    return v;
}

// free(NULL): "nop"
//
#define FREE(P) free_0(&(P))
inline static void free_0(void *p)
{
    free(*(void **)p);
    *(void **)p = NULL;
}

// T: Pointer Type
#define SIZE_OF(T) (sizeof *(T){0})
#define ZERO_OF(T) ((T) & (char[SIZE_OF(T)]){})

#endif // NSTD_MEMORY_H
