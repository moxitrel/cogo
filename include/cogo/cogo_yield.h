// clang-format off
/*

* API
CO_BEGIN      : ...
CO_END        : ...
CO_YIELD      : ...
CO_RETURN     : ...
co_this       : ...
co_status(CO) : ...

CO_DECLARE(NAME, ...)   : declare a coroutine.
CO_DEFINE (NAME)        : define a declared coroutine which not defined.
CO_MAKE   (NAME, ...)   : coroutine maker.
NAME_func               : coroutine function name, made by CO_DECLARE(NAME), e.g. Nat_func

*/
#ifndef COGO_COGO_YIELD_H_
#define COGO_COGO_YIELD_H_

#if defined(COGO_YIELD_CASE)
    #include "cogo_yield_case.h"
#elif defined(COGO_YIELD_LABEL_VALUE) || defined(__GNUC__)
    #include "cogo_yield_label_value.h"
#else
    #include "cogo_yield_case.h"
#endif

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

// COGO_STRUCT(Type, T1 field1, ...): define a struct named <Type>
//
// * Example
// COGO_STRUCT(Point, int x, int y, int z) will be expanded into
//  typedef struct {
//      int x;
//      int y;
//      int z;
//  } Point
//
#define COGO_COMMA_static ,
#define COGO_COMMA_extern ,
#define COGO_REMOVE_LINKAGE_static
#define COGO_REMOVE_LINKAGE_extern
#define COGO_ID(X)                  X
#define COGO_STRUCT(NAME, ...)      COGO_STRUCT1(COGO_ARG_COUNT(COGO_COMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_STRUCT1(...)           COGO_STRUCT2(__VA_ARGS__)
#define COGO_STRUCT2(N, ...)        COGO_STRUCT3_##N(__VA_ARGS__)
#define COGO_STRUCT3_1(NAME, ...) /* NAME: Type */ \
    typedef struct NAME NAME##_t;                  \
    struct NAME {                                  \
        COGO_MAP(;, COGO_ID, __VA_ARGS__);         \
    }
#define COGO_STRUCT3_2(NAME, ...) /* NAME: static Type */                     \
    typedef struct COGO_REMOVE_LINKAGE_##NAME COGO_REMOVE_LINKAGE_##NAME##_t; \
    struct COGO_REMOVE_LINKAGE_##NAME {                                       \
        COGO_MAP(;, COGO_ID, __VA_ARGS__);                                    \
    }

#define COGO_DECLARE(NAME, BASE, ...)                                                                 \
    COGO_IFNIL(COGO_ARG(__VA_ARGS__), COGO_STRUCT(NAME, BASE), COGO_STRUCT(NAME, BASE, __VA_ARGS__)); \
    CO_DEFINE(NAME)

#define CO_DEFINE(NAME)         \
    void NAME##_func(void* co_this)

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...)   \
    COGO_DECLARE(NAME, cogo_yield_t cogo_yield, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...)                              \
_Pragma("GCC diagnostic push")                          \
_Pragma("GCC diagnostic ignored \"-Wc99-extensions\"")  \
    ((NAME##_t){{}, __VA_ARGS__})                      \
_Pragma("GCC diagnostic pop")

#ifdef __cplusplus
}
#endif
#endif /* COGO_COGO_YIELD_H_ */
