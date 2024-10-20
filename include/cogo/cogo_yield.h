/*

* API
co_this     : void*, point to coroutine object.
CO_BEGIN    : coroutine begin label.
CO_END      : coroutine end label.
CO_YIELD    : yield from coroutine.
CO_RETURN   : return from coroutine.

cogo_status_t       : type of COGO_STATUS().
COGO_STATUS()       : get the current running status.
  COGO_STATUS_BEGIN : inited
  COGO_STATUS_END   : finished

CO_DECLARE(NAME, ...){} : declare a coroutine.
  FUNC_t                : the declared coroutine type.
  FUNC_resume           : the declared coroutine function.
CO_DEFINE (NAME)     {} : define a declared coroutine which is not defined.

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

#define COGO_COMMA_static ,
#define COGO_BLANK_static

// COGO_STRUCT(point, int x, int y):
//  typedef struct point {
//      int x;
//      int y;
//  } point_t
#define COGO_STRUCT(NAME, ...)           \
  typedef struct NAME NAME##_t;          \
  struct NAME {                          \
    ET_MAP(;, ET_IDENTITY, __VA_ARGS__); \
  }

// typedef struct NAME NAME_t;
// struct NAME {
//  BASE_FIELD;
//  ...
// };
// void NAME_resume(void* const co_this)
#define COGO_DECLARE(NAME, BASE_FIELD, ...)    COGO_DECLARE1(ET_COUNT(COGO_COMMA_##NAME), NAME, BASE_FIELD, __VA_ARGS__)
#define COGO_DECLARE1(...)                     COGO_DECLARE2(__VA_ARGS__)
#define COGO_DECLARE2(N, ...)                  COGO_DECLARE3_##N(__VA_ARGS__)
#define COGO_DECLARE3_1(NAME, BASE_FIELD, ...) /* NAME: name */ \
  ET_IF_NIL(ET_IDENTITY(__VA_ARGS__),                           \
            COGO_STRUCT(NAME, BASE_FIELD),                      \
            COGO_STRUCT(NAME, BASE_FIELD, __VA_ARGS__));        \
  CO_DEFINE(NAME)
#define COGO_DECLARE3_2(NAME, BASE_FIELD, ...) /* NAME: static name */ \
  ET_IF_NIL(ET_IDENTITY(__VA_ARGS__),                                  \
            COGO_STRUCT(COGO_BLANK_##NAME, BASE_FIELD),                \
            COGO_STRUCT(COGO_BLANK_##NAME, BASE_FIELD, __VA_ARGS__));  \
  CO_DEFINE(NAME)

typedef cogo_pc_t cogo_status_t;
#define COGO_STATUS_BEGIN COGO_PC_BEGIN
#define COGO_STATUS_END   COGO_PC_END
#define COGO_STATUS(CO)   ((cogo_status_t)COGO_PC(CO))  // get as rvalue

/// Coroutine begin label.
#define CO_BEGIN          COGO_BEGIN(co_this)
/// Coroutine end label.
#define CO_END            COGO_END(co_this)
/// Jump to `COGO_END`, and the next run will start from here.
#define CO_YIELD          COGO_YIELD(co_this)
/// Jump to `COGO_END`, and end the coroutine.
#define CO_RETURN         COGO_RETURN(co_this)

#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_yield_t base_yield, __VA_ARGS__)

#define CO_DEFINE(NAME)    CO_DEFINE1(ET_COUNT(COGO_COMMA_##NAME), NAME)
#define CO_DEFINE1(...)    CO_DEFINE2(__VA_ARGS__)
#define CO_DEFINE2(N, ...) CO_DEFINE3_##N(__VA_ARGS__)
#define CO_DEFINE3_1(NAME) void NAME##_resume(void* const co_this)
#define CO_DEFINE3_2(NAME) static void COGO_BLANK_##NAME##_resume(void* const co_this)

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
