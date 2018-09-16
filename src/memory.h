/*
 * void *MALLOC (size_t)        : =malloc
 * void *CALLOC (size_t, size_t): =calloc
 * void  FREE   (T *)           : =free
 *
 * */
#ifndef NSTD_MEMORY_H
#define NSTD_MEMORY_H

#include "type.h"
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(E, ...)                                                  \
(                                                                       \
    (E) ? (void)0                                                       \
        : ( printf("[%s %s %u] %s", __FILE__, __func__, __LINE__, #E)   \
          , printf(": " __VA_ARGS__)  /* append string literal */       \
          , printf("\n")                                                \
          , exit(EXIT_FAILURE)        /* abort() */                     \
          )                                                             \
)

// alignas(N|T) char (*p)[N];   // object of compatible type
//
#define MALLOC(N) malloc_assert((N), __FILE__, __func__, __LINE__)
inline static void *malloc_assert(size_t n, const char *file, const char *func, int line)
{
    void *v = malloc(n);
    ASSERT(v, "ENOMEM, malloc(%zd)", n);
    return v;
}

#define CALLOC(N, M) calloc_assert((N), (M), __FILE__, __func__, __LINE__)
inline static void *calloc_assert(size_t n, size_t m, const char *file, const char *func, int line)
{
    void *v = calloc(n, m);
    ASSERT(v, "ENOMEM, calloc(%zd)", n);
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

// use calloc() to init
typedef struct {
    void * ptr;
    size_t size;
    size_t cap;
} Object;

// size: 0, release the memory held by Object.ptr
inline static Object *Object_Resize(Object *o, size_t size)
{
    assert(o);
    if (size > o->cap) {
        o->cap = size;
        free(o->ptr);
        o->ptr = MALLOC(o->cap);
    }
    o->size = size;
    return o;
}

inline static void Object_Gc(Object *o)
{
    o->cap = 0;
    o->size = 0;
    FREE(o->ptr);
}

#endif // NSTD_MEMORY_H
