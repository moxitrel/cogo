/*
 * void *NEW    (uint n): allocate memory of size n
 * void  DELETE (T *)   : release memory
 *
 * noreturn void ASSERT_MSG(exp, char msg[])
 *   abort with <msg> if <exp> eval to false.
 *
 * */
#ifndef NSTD_MEMORY_H
#define NSTD_MEMORY_H

#include "type.h"
#include <stdlib.h>
#include <stdio.h>

//#define ASSERT_MSG(E, ...)                                                                  \
//(                                                                                           \
//    (E) ? (void)0                                                                           \
//        : ( printf("[%s %s %u] %s: %s\n", __FILE__, __func__, __LINE__, #E, #__VA_ARGS__)   \
//          , abort()                                                                         \
//          )                                                                                 \
//)

// alignas(N|T) char (*p)[N];   // object of compatible type
//
// void *NEW(uint): allocate memory, =C++ operator new()
// e.g. T *p    = NEW(sizeof *p);
//      T *p[n] = NEW(sizeof *p * n);
#define NEW(N)                                      \
(                                                   \
    malloc(N)   ||                                  \
    ( printf("[%s %s %u] ENOMEM: malloc(%s)\n",     \
            __FILE__, __func__, __LINE__, #N)       \
    , abort()                                       \
    , NULL                                          \
    )                                               \
)

// free(NULL): "nop"
//
// void DELETE(T *): release memory, =C++ operator delete()
#define DELETE(P) ((void)(free(P), (P) = NULL))


// T: Pointer Type
#define SIZE_OF(T)   (sizeof *(T){0})
#define ZERO_OF(T)   ((T)&(char[SIZE_OF(T)]){})

#endif //NSTD_MEMORY_H
