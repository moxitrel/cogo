/*

* API
cogo_this     : void*, point to coroutine object.
CO_BEGIN    : coroutine begin label.
CO_END      : coroutine end label.
CO_YIELD    : yield from coroutine.
CO_RETURN   : return from coroutine.

co_status_t       : type of CO_STATUS().
CO_STATUS()       : get the current running status.
  CO_STATUS_BEGIN : inited
  CO_STATUS_END   : finished

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

// COGO_STRUCT1(point, int x, int y):
//  typedef struct point {
//      int x;
//      int y;
//  } point_t
#define COGO_STRUCT1(NAME, ...)          \
  typedef struct NAME NAME##_t;          \
  struct NAME {                          \
    ET_MAP(;, ET_IDENTITY, __VA_ARGS__); \
  }

// typedef struct NAME NAME_t;
// struct NAME {
//  BASE_FIELD;
//  ...
// };
// void NAME_resume(void* const cogo_this)
#define COGO_DECLARE(NAME, BASE_FIELD, ...)      COGO_DECLARE_F1(ET_COUNT(COGO_COMMA_##NAME), NAME, BASE_FIELD, __VA_ARGS__)
#define COGO_DECLARE_F1(...)                     COGO_DECLARE_F2(__VA_ARGS__)
#define COGO_DECLARE_F2(N, ...)                  COGO_DECLARE_F3_##N(__VA_ARGS__)
#define COGO_DECLARE_F3_1(NAME, BASE_FIELD, ...) /* NAME: name */ \
  ET_IF_NIL(ET_IDENTITY(__VA_ARGS__),                             \
            COGO_STRUCT1(NAME, BASE_FIELD),                       \
            COGO_STRUCT1(NAME, BASE_FIELD, __VA_ARGS__));         \
  CO_DEFINE(NAME)
#define COGO_DECLARE_F3_2(NAME, BASE_FIELD, ...) /* NAME: static name */ \
  ET_IF_NIL(ET_IDENTITY(__VA_ARGS__),                                    \
            COGO_STRUCT1(COGO_BLANK_##NAME, BASE_FIELD),                 \
            COGO_STRUCT1(COGO_BLANK_##NAME, BASE_FIELD, __VA_ARGS__));   \
  CO_DEFINE(NAME)

#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_yield_t base_yield, __VA_ARGS__)

#define CO_DEFINE(NAME)      CO_DEFINE_F1(ET_COUNT(COGO_COMMA_##NAME), NAME)
#define CO_DEFINE_F1(...)    CO_DEFINE_F2(__VA_ARGS__)
#define CO_DEFINE_F2(N, ...) CO_DEFINE_F3_##N(__VA_ARGS__)
#define CO_DEFINE_F3_1(NAME) void NAME##_resume(void* const cogo_this)
#define CO_DEFINE_F3_2(NAME) static void COGO_BLANK_##NAME##_resume(void* const cogo_this)

#define CO_BEGIN             COGO_BEGIN(cogo_this)
#define CO_END               COGO_END(cogo_this)
#define CO_YIELD             COGO_YIELD(cogo_this)
#define CO_RETURN            COGO_RETURN(cogo_this)

typedef cogo_pc_t co_status_t;
#define CO_STATUS_BEGIN COGO_PC_BEGIN
#define CO_STATUS_END   COGO_PC_END
#define CO_STATUS(CO)   ((co_status_t)COGO_PC(CO))

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
