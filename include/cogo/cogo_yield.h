/*

* API
co_this
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN

NAME_t                  : coroutine type created by CO_DECLARE()
CO_MAKE(NAME, ...)      : make a new coroutine
co_status_t
CO_STATUS()
CO_RESUME()             : continue to run a suspended coroutine until yield or finished
CO_RUN()                : run the coroutine and all other created coroutines until finished

CO_DECLARE(NAME, ...){} : declare a coroutine.
CO_DEFINE(NAME){}       : define a declared coroutine which not defined.

*/
#ifndef COGO_YIELD_H_
#define COGO_YIELD_H_

#include "_private/macro_utils.h"

#if defined(COGO_USE_LABEL_VALUE)
#include "_private/cogo_yield_label_value.h"
#elif defined(COGO_USE_CASE)
#include "_private/cogo_yield_case.h"
#elif defined(__GNUC__)
#include "_private/cogo_yield_label_value.h"
#else
#include "_private/cogo_yield_case.h"
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
#define COGO_COMMA_static ,
#define COGO_REMOVE_LINKAGE_static
#define COGO_STRUCT(NAME, ...)    COGO_STRUCT1(CX0_COUNT(COGO_COMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_STRUCT1(...)         COGO_STRUCT2(__VA_ARGS__)
#define COGO_STRUCT2(N, ...)      COGO_STRUCT3_##N(__VA_ARGS__)
#define COGO_STRUCT3_1(NAME, ...) /* NAME: Type */ \
  typedef struct NAME NAME##_t;                    \
  struct NAME {                                    \
    CX0_MAP(;, CX0_IDENTITY, __VA_ARGS__);         \
  }
#define COGO_STRUCT3_2(NAME, ...) /* NAME: static Type */                   \
  typedef struct COGO_REMOVE_LINKAGE_##NAME COGO_REMOVE_LINKAGE_##NAME##_t; \
  struct COGO_REMOVE_LINKAGE_##NAME {                                       \
    CX0_MAP(;, CX0_IDENTITY, __VA_ARGS__);                                  \
  }

#define COGO_DECLARE(NAME, BASE, ...)               \
  CX0_IF_NIL(CX0_IDENTITY(__VA_ARGS__),             \
             COGO_STRUCT(NAME, BASE),               \
             COGO_STRUCT(NAME, BASE, __VA_ARGS__)); \
  CO_DEFINE(NAME)

#define CO_DEFINE(NAME)    CO_DEFINE1(CX0_COUNT(COGO_COMMA_##NAME), NAME)
#define CO_DEFINE1(...)    CO_DEFINE2(__VA_ARGS__)
#define CO_DEFINE2(N, ...) CO_DEFINE3_##N(__VA_ARGS__)
#define CO_DEFINE3_1(NAME) void NAME##_func(void* const co_this)
#define CO_DEFINE3_2(NAME) static void COGO_REMOVE_LINKAGE_##NAME##_func(void* const co_this)

#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_yield_t base, __VA_ARGS__)

#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.func = NAME##_func}, __VA_ARGS__})

// continue to run a suspended coroutine until yield or finished
#define CO_RESUME(CO) cogo_yield_resume((cogo_yield_t*)(CO))
co_status_t cogo_yield_resume(cogo_yield_t* co);

// run the coroutines until all finished
#define CO_RUN(CO) cogo_yield_run((cogo_yield_t*)(CO))
void cogo_yield_run(cogo_yield_t* co);

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
