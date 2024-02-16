/*

* API
co_this
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN

CO_INITER(CO, TYPE, ...): make a new coroutine
co_status_t
CO_STATUS(CO)

CO_DECLARE(TYPE, ...){} : declare a coroutine.
CO_DEFINE (TYPE)     {} : define a declared coroutine which not defined.

*/
#ifndef COGO_YIELD_H_
#define COGO_YIELD_H_

#include "_internal/macro_utils.h"

#if defined(COGO_USE_LABELS_AS_VALUES)
#include "_internal/cogo_yield_labels_as_values.h"
#elif defined(COGO_USE_SWITCH)
#include "_internal/cogo_yield_switch.h"
#elif defined(__GNUC__)
#include "_internal/cogo_yield_labels_as_values.h"
#else
#include "_internal/cogo_yield_switch.h"
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
#define COGO_STRUCT(TYPE, ...)    COGO_STRUCT1(CX0_COUNT(COGO_COMMA_##TYPE), TYPE, __VA_ARGS__)
#define COGO_STRUCT1(...)         COGO_STRUCT2(__VA_ARGS__)
#define COGO_STRUCT2(N, ...)      COGO_STRUCT3_##N(__VA_ARGS__)
#define COGO_STRUCT3_1(TYPE, ...) /* TYPE: name_t */ \
  typedef struct TYPE TYPE;                          \
  struct TYPE {                                      \
    CX0_MAP(;, CX0_IDENTITY, __VA_ARGS__);           \
  }
#define COGO_STRUCT3_2(TYPE, ...) /* TYPE: static name_t */             \
  typedef struct COGO_REMOVE_LINKAGE_##TYPE COGO_REMOVE_LINKAGE_##TYPE; \
  struct COGO_REMOVE_LINKAGE_##TYPE {                                   \
    CX0_MAP(;, CX0_IDENTITY, __VA_ARGS__);                              \
  }

#define COGO_DECLARE(TYPE, BASE, ...)               \
  CX0_IF_NIL(CX0_IDENTITY(__VA_ARGS__),             \
             COGO_STRUCT(TYPE, BASE),               \
             COGO_STRUCT(TYPE, BASE, __VA_ARGS__)); \
  CO_DEFINE(TYPE)

#define CO_DEFINE(TYPE)    CO_DEFINE1(CX0_COUNT(COGO_COMMA_##TYPE), TYPE)
#define CO_DEFINE1(...)    CO_DEFINE2(__VA_ARGS__)
#define CO_DEFINE2(N, ...) CO_DEFINE3_##N(__VA_ARGS__)
#define CO_DEFINE3_1(TYPE) void TYPE##_resume(void* const co_this)
#define CO_DEFINE3_2(TYPE) static void COGO_REMOVE_LINKAGE_##TYPE##_resume(void* const co_this)

#define CO_DECLARE(TYPE, ...) \
  COGO_DECLARE(TYPE, cogo_yield_t base, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
