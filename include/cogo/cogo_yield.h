/*

* API
cogo_this     : void*, point to coroutine object.
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
#include "private/cogo_pt_case.h"
#elif defined(__GNUC__)
#include "private/cogo_pt_goto.h"
#else
#include "private/cogo_pt_case.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#undef COGO_T
#define COGO_T cogo_yield_t
typedef struct cogo_yield cogo_yield_t;
struct cogo_yield {
  cogo_pt_t private_pt;
  void (*resume)(COGO_T* cogo_this);
};

#undef COGO_PC
#define COGO_PC(YIELD) ((YIELD)->private_pt.private_pc)

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

#define COGO_COMMA_static ,
#define COGO_BLANK_static

// typedef struct NAME NAME_t;
// struct NAME {
//  PARAM1;
//  ...
// };
// void NAME_resume(NAME_t* const cogo_this)
#define COGO_DECLARE1(NAME, ...)      COGO_DECLARE1_F1(ET_COUNT(COGO_COMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_DECLARE1_F1(...)         COGO_DECLARE1_F2(__VA_ARGS__)
#define COGO_DECLARE1_F2(N, ...)      COGO_DECLARE1_F3_##N(__VA_ARGS__)
#define COGO_DECLARE1_F3_1(NAME, ...) /* NAME: name */ \
  COGO_STRUCT1(NAME, __VA_ARGS__);                     \
  void NAME##_resume(COGO_T* const cogo_this)
#define COGO_DECLARE1_F3_2(NAME, ...) /* NAME: static name */ \
  COGO_STRUCT1(COGO_BLANK_##NAME, __VA_ARGS__);               \
  static void COGO_BLANK_##NAME##_resume(COGO_T* const cogo_this)

#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE1(NAME, COGO_T private_yield, __VA_ARGS__)

#define CO_DEFINE(NAME)      CO_DEFINE_F1(ET_COUNT(COGO_COMMA_##NAME), NAME)
#define CO_DEFINE_F1(...)    CO_DEFINE_F2(__VA_ARGS__)
#define CO_DEFINE_F2(N, ...) CO_DEFINE_F3_##N(__VA_ARGS__)
#define CO_DEFINE_F3_1(NAME) void NAME##_resume(COGO_T* const cogo_this)                      // NAME: name
#define CO_DEFINE_F3_2(NAME) static void COGO_BLANK_##NAME##_resume(COGO_T* const cogo_this)  // NAME: static name

#undef COGO_PT
#define COGO_PT (&cogo_this->private_pt)

typedef cogo_pc_t cogo_status_t;
#define COGO_STATUS_BEGIN  COGO_PC_BEGIN
#define COGO_STATUS_END    COGO_PC_END
#define COGO_STATUS(YIELD) (+COGO_PC((COGO_T*)YIELD))

#define COGO_RESUME(YIELD) cogo_yield_resume((COGO_T*)YIELD)
static inline cogo_status_t cogo_yield_resume(COGO_T* cogo_this) {
  if (COGO_PC(cogo_this) != COGO_PC_END) {
    cogo_this->resume(cogo_this);
  }
  return COGO_PC(cogo_this);
}

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
