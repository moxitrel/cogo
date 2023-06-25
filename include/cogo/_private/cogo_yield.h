/*

* API
co_this
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END

co_status()
CO_DECLARE(NAME, ...){...}  : declare a coroutine.
CO_DEFINE(NAME){...}        : define a declared coroutine which not defined.
CO_MAKE(NAME, ...)          : make a new coroutine
NAME_t                      : coroutine type created by CO_DECLARE()

NAME_func                   : coroutine function created by CO_DECLARE()

*/
#ifndef COGO_YIELD_H_
#define COGO_YIELD_H_

#include "macro_utils.h"

#if defined(COGO_USE_CASE)
#include "cogo_yield_case.h"
#elif defined(COGO_USE_LABEL_VALUE) || defined(__GNUC__)
#include "cogo_yield_label_value.h"
#else
#include "cogo_yield_case.h"
#endif

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
#define COGO_ASYNCMMA_static ,
#define COGO_REMOVE_LINKAGE_static
#define COGO_STRUCT(NAME, ...)    COGO_STRUCT1(CX2_COUNT(COGO_ASYNCMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_STRUCT1(...)         COGO_STRUCT2(__VA_ARGS__)
#define COGO_STRUCT2(N, ...)      COGO_STRUCT3_##N(__VA_ARGS__)
#define COGO_STRUCT3_1(NAME, ...) /* NAME: Type */ \
  typedef struct NAME NAME##_t;                    \
  struct NAME {                                    \
    CX2_MAP(;, CX2_IDENTITY, __VA_ARGS__);         \
  }
#define COGO_STRUCT3_2(NAME, ...) /* NAME: static Type */                   \
  typedef struct COGO_REMOVE_LINKAGE_##NAME COGO_REMOVE_LINKAGE_##NAME##_t; \
  struct COGO_REMOVE_LINKAGE_##NAME {                                       \
    CX2_MAP(;, COGO_ID, __VA_ARGS__);                                       \
  }

#define COGO_DECLARE(NAME, BASE, ...)               \
  CX2_IF_NIL(CX2_IDENTITY(__VA_ARGS__),             \
             COGO_STRUCT(NAME, BASE),               \
             COGO_STRUCT(NAME, BASE, __VA_ARGS__)); \
  CO_DEFINE(NAME)

#define CO_DEFINE(NAME)    CO_DEFINE1(CX2_COUNT(COGO_ASYNCMMA_##NAME), NAME)
#define CO_DEFINE1(...)    CO_DEFINE2(__VA_ARGS__)
#define CO_DEFINE2(N, ...) CO_DEFINE3_##N(__VA_ARGS__)
#define CO_DEFINE3_1(NAME) void NAME##_func(void* const co_this)
#define CO_DEFINE3_2(NAME) static void COGO_REMOVE_LINKAGE_##NAME##_func(void* const co_this)

#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_yield_t super, __VA_ARGS__)

#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{0}, __VA_ARGS__})

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
