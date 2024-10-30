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

typedef struct cogo_yield cogo_yield_t;
struct cogo_yield {
  cogo_pt_t private_pt;
  void (*resume)(COGO_T* cogo_this);
};

#undef COGO_T
#define COGO_T cogo_yield_t

// COGO_STRUCT(point, int x, int y):
//  typedef struct point {
//      int x;
//      int y;
//  } point_t
#define COGO_STRUCT(NAME, BASE, ...)       \
  typedef struct NAME NAME##_t;            \
  struct NAME {                            \
    BASE;                                  \
    struct {                               \
      ET_MAP(;, ET_IDENTITY, __VA_ARGS__); \
    } cogo_this;                           \
  }

#define COGO_COMMA_static ,
#define COGO_BLANK_static

// typedef struct NAME NAME_t;
// struct NAME {
//  PARAM1;
//  ...
// };
// void NAME_resume(NAME_t* const cogo_this)
#define COGO_DECLARE(NAME, ...)        COGO_DO_DECLARE1(ET_HAS_COMMA(COGO_COMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_DO_DECLARE1(...)          COGO_DO_DECLARE2(__VA_ARGS__)
#define COGO_DO_DECLARE2(N1, ...)      COGO_DO_DECLARE3_##N1(__VA_ARGS__) COGO_DEFINE(NAME)
#define COGO_DO_DECLARE3_0(NAME, ...)  COGO_DECLARE_EXTERN(NAME, __VA_ARGS__)  // NAME: name
#define COGO_DO_DECLARE3_1(NAME, ...)  COGO_DECLARE_STATIC(NAME, __VA_ARGS__)  // NAME: static name
#define COGO_DECLARE_EXTERN(NAME, ...) COGO_STRUCT(NAME, COGO_T cogo_pt, __VA_ARGS__);
#define COGO_DECLARE_STATIC(NAME, ...) COGO_STRUCT(COGO_BLANK_##NAME, COGO_T cogo_pt, __VA_ARGS__);

#define COGO_DEFINE(NAME)              COGO_DO_DEFINE1(ET_HAS_COMMA(COGO_COMMA_##NAME), NAME)
#define COGO_DO_DEFINE1(...)           COGO_DO_DEFINE2(__VA_ARGS__)
#define COGO_DO_DEFINE2(N, ...)        COGO_DO_DEFINE3_##N(__VA_ARGS__)
#define COGO_DO_DEFINE3_0(NAME)        COGO_DEFINE_EXTERN(NAME)  // NAME: name
#define COGO_DO_DEFINE3_1(NAME)        COGO_DEFINE_STATIC(NAME)  // NAME: static name
#define COGO_DEFINE_EXTERN(NAME)       void NAME##_resume(void* const cogo_this)
#define COGO_DEFINE_STATIC(NAME)       static void COGO_BLANK_##NAME##_resume(void* const cogo_this)

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
