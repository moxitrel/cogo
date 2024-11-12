/*

* API
cogo_this   : void*, point to coroutine object.
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
COGO_DEFINE (NAME)     {} : define a declared coroutine which is not defined.

*/
#ifndef COGO_YIELD_H_
#define COGO_YIELD_H_

#include "private/macro_utils.h"

#if defined(COGO_NO_LABELS_AS_VALUES)
#include "private/cogo_pt_case.h"
#elif defined(__GNUC__)
#include "private/cogo_pt_goto.h"
#else
#include "private/cogo_pt_case.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...)  // noop
#endif

#ifndef COGO_T
#define COGO_T cogo_yield_t
typedef struct cogo_yield cogo_yield_t;
#endif

#define COGO_COMMA_static ,
#define COGO_COMMA_extern ,
#define COGO_BLANK_static
#define COGO_BLANK_extern

typedef struct cogo_yield cogo_yield_t;
struct cogo_yield {
  cogo_pt_t base_pt;
  void (*resume)(COGO_T* cogo_this);
};

// COGO_STRUCT(point, int x, int y):
//  typedef struct point {
//      int x;
//      int y;
//  } point_t
#define COGO_STRUCT(NAME, ...)  COGO_DO_STRUCT1(ET_IS_EMPTY(__VA_ARGS__), NAME, __VA_ARGS__)
#define COGO_DO_STRUCT1(...)    COGO_DO_STRUCT2(__VA_ARGS__)
#define COGO_DO_STRUCT2(N, ...) COGO_DO_STRUCT3_##N(__VA_ARGS__)
#define COGO_DO_STRUCT3_0(NAME, ...)     \
  typedef struct NAME NAME##_t;          \
  struct NAME {                          \
    COGO_T cogo;                         \
    ET_MAP(;, ET_IDENTITY, __VA_ARGS__); \
  }
#define COGO_DO_STRUCT3_1(NAME, ...) \
  typedef struct NAME {              \
    COGO_T cogo;                     \
  } NAME##_t

// typedef struct NAME NAME_t;
// struct NAME {
//  COGO_T cogo;
//  ...
// };
// void NAME_resume(NAME_t* const cogo_this)
#define COGO_DECLARE(NAME, ...) COGO_DO_DECLARE1(ET_HAS_COMMA(COGO_COMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_DO_DECLARE1(...)   COGO_DO_DECLARE2(__VA_ARGS__)
#define COGO_DO_DECLARE2(N, NAME, ...)     \
  COGO_DO_DECLARE3_##N(NAME, __VA_ARGS__); \
  COGO_DEFINE(NAME)
#define COGO_DO_DECLARE3_0(NAME, ...) COGO_STRUCT(NAME, __VA_ARGS__)               // NAME: name
#define COGO_DO_DECLARE3_1(NAME, ...) COGO_STRUCT(COGO_BLANK_##NAME, __VA_ARGS__)  // NAME: static name

#define COGO_DEFINE(NAME)             COGO_DO_DEFINE1(ET_HAS_COMMA(COGO_COMMA_##NAME), NAME)
#define COGO_DO_DEFINE1(...)          COGO_DO_DEFINE2(__VA_ARGS__)
#define COGO_DO_DEFINE2(N, ...)       COGO_DO_DEFINE3_##N(__VA_ARGS__)
#define COGO_DO_DEFINE3_0(NAME)       void NAME##_resume(COGO_T* const cogo_this)                      // NAME: name
#define COGO_DO_DEFINE3_1(NAME)       static void COGO_BLANK_##NAME##_resume(COGO_T* const cogo_this)  // NAME: static name

#define COGO_INIT(NAME, THIZ, ...)             \
  ((NAME##_t){                                 \
      /* .cogo = */ {.resume = NAME##_resume}, \
      __VA_ARGS__})

// Continue to run a suspended coroutine until yield or finished.
#define COGO_RESUME(COGO) cogo_yield_resume(&(COGO)->cogo)
cogo_pc_t cogo_yield_resume(cogo_yield_t* cogo);

#undef CO_BEGIN
#undef CO_END
#undef CO_YIELD
#undef CO_RETURN
#define COGO_PT_V(COGO_THIS) (&(COGO_THIS)->base_pt)
#define CO_BEGIN             COGO_BEGIN(COGO_PT_V(cogo_this))
#define CO_END               COGO_END(COGO_PT_V(cogo_this))
#define CO_YIELD             COGO_YIELD(COGO_PT_V(cogo_this))
#define CO_RETURN            COGO_RETURN(COGO_PT_V(cogo_this))

#define COGO_STATUS(COGO)    COGO_PC(COGO_PT_V(&(COGO)->cogo))

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
