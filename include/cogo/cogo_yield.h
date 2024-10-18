/*

* API
co_this     : void*, point to coroutine object.
CO_BEGIN    : coroutine begin label.
CO_END      : coroutine end label.
CO_YIELD    : yield from coroutine.
CO_RETURN   : return from coroutine.

co_status_t       : type of CO_STATUS().
CO_STATUS()       : get the current running status.
  CO_STATUS_BEGIN : inited
  CO_STATUS_END   : finished

CO_DECLARE(FUNC, ...){} : declare a coroutine.
  FUNC_t                : the declared coroutine type.
  FUNC_resume           : the declared coroutine function.
CO_DEFINE (FUNC)     {} : define a declared coroutine which is not defined.

*/
#ifndef COGO_YIELD_H_
#define COGO_YIELD_H_

#include "private/macro_utils.h"

#if defined(COGO_NO_LABELS_AS_VALUES)
#include "private/cogo_yield_case.h"
#elif defined(__GNUC__)
#include "private/cogo_yield_goto.h"
#else
#include "private/cogo_yield_case.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// COGO_STRUCT(Type, T1 field1, ...): define a struct named <Type>
//
// * Example
// COGO_STRUCT(point, int x, int y, int z) will be expanded to
//  typedef struct point {
//      int x;
//      int y;
//      int z;
//  } point_t
//
#define COGO_COMMA_static ,
#define COGO_BLANK_static
#define COGO_STRUCT(TYPE, ...)    COGO_STRUCT1(ET_COUNT(COGO_COMMA_##TYPE), TYPE, __VA_ARGS__)
#define COGO_STRUCT1(...)         COGO_STRUCT2(__VA_ARGS__)
#define COGO_STRUCT2(N, ...)      COGO_STRUCT3_##N(__VA_ARGS__)
#define COGO_STRUCT3_1(TYPE, ...) /* TYPE: name */ \
  typedef struct TYPE TYPE##_t;                    \
  struct TYPE {                                    \
    ET_MAP(;, ET_IDENTITY, __VA_ARGS__);           \
  }
#define COGO_STRUCT3_2(TYPE, ...) /* TYPE: static name */ \
  typedef struct COGO_BLANK_##TYPE COGO_BLANK_##TYPE##_t; \
  struct COGO_BLANK_##TYPE {                              \
    ET_MAP(;, ET_IDENTITY, __VA_ARGS__);                  \
  }

#define COGO_DECLARE(FUNC, BASE, ...)              \
  ET_IF_NIL(ET_IDENTITY(__VA_ARGS__),              \
            COGO_STRUCT(FUNC, BASE),               \
            COGO_STRUCT(FUNC, BASE, __VA_ARGS__)); \
  CO_DEFINE(FUNC)

#define CO_DEFINE(FUNC)    CO_DEFINE1(ET_COUNT(COGO_COMMA_##FUNC), FUNC)
#define CO_DEFINE1(...)    CO_DEFINE2(__VA_ARGS__)
#define CO_DEFINE2(N, ...) CO_DEFINE3_##N(__VA_ARGS__)
#define CO_DEFINE3_1(FUNC) void FUNC##_resume(void* const co_this)
#define CO_DEFINE3_2(FUNC) static void COGO_BLANK_##FUNC##_resume(void* const co_this)

// typedef struct FUNC FUNC_t;
// struct FUNC {
//  cogo_yield_t base_yield;
//  ...
// };
// void FUNC_resume(void* const co_this)
#define CO_DECLARE(FUNC, ...) \
  COGO_DECLARE(FUNC, cogo_yield_t base_yield, __VA_ARGS__)

#define CO_BEGIN  COGO_BEGIN(co_this)
#define CO_END    COGO_END(co_this)
#define CO_YIELD  COGO_YIELD(co_this)
#define CO_RETURN COGO_RETURN(co_this)

typedef cogo_pc_t co_status_t;
#define CO_STATUS_BEGIN COGO_PC_BEGIN
#define CO_STATUS_END   COGO_PC_END
#define CO_STATUS(CO)   ((co_status_t)COGO_PC(CO))  // get as rvalue

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
