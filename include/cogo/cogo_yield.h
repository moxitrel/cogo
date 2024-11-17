/*

* API
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

#ifndef COGO_T
#define COGO_T cogo_yield_t
typedef struct cogo_yield cogo_yield_t;
#endif

#include "private/macro_utils.h"

#if defined(COGO_NO_COMPUTED_GOTO)
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

typedef struct cogo_yield cogo_yield_t;
struct cogo_yield {
  /// @private The base type.
  cogo_pt_t base_pt;
  /// @protected The coroutine function.
  void (*resume)(COGO_T* cogo_this);
};

#define COGO_YIELD_INITIALIZER(NAME) \
  { .resume = NAME##_resume }

#undef COGO_PT_V
#define COGO_YIELD_V(YIELD)     (YIELD)
#define COGO_PT_V(COGO)         (&COGO_YIELD_V(COGO)->base_pt)

// typedef struct NAME NAME_t;
// struct NAME {
//  COGO_T base_cogo;
//  ...
// };
// void NAME_resume(NAME_t* const cogo_this)
#define COGO_DECLARE(NAME, ...) COGO_DO_DECLARE1(ZY_HAS_COMMA(COGO_COMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_DO_DECLARE1(...)   COGO_DO_DECLARE2(__VA_ARGS__)
#define COGO_DO_DECLARE2(N, NAME, ...)     \
  COGO_DO_DECLARE3_##N(NAME, __VA_ARGS__); \
  COGO_DEFINE(NAME)
#define COGO_DO_DECLARE3_0(NAME, ...) COGO_STRUCT(NAME, __VA_ARGS__)               // NAME: name
#define COGO_DO_DECLARE3_1(NAME, ...) COGO_STRUCT(COGO_BLANK_##NAME, __VA_ARGS__)  // NAME: static name

#define COGO_STRUCT(NAME, ...)        COGO_DO_STRUCT1(ZY_IS_EMPTY(__VA_ARGS__), NAME, __VA_ARGS__)
#define COGO_DO_STRUCT1(...)          COGO_DO_STRUCT2(__VA_ARGS__)
#define COGO_DO_STRUCT2(N, ...)       COGO_DO_STRUCT3_##N(__VA_ARGS__)
#define COGO_DO_STRUCT3_0(NAME, ...)      \
  typedef struct NAME NAME##_t;           \
  struct NAME {                           \
    COGO_T base_cogo;                     \
    ZY_MAP1(;, ZY_IDENTITY, __VA_ARGS__); \
  }
#define COGO_DO_STRUCT3_1(NAME, ...) \
  typedef struct NAME {              \
    COGO_T base_cogo;                \
  } NAME##_t

#define COGO_DEFINE(NAME)       COGO_DO_DEFINE1(ZY_HAS_COMMA(COGO_COMMA_##NAME), NAME)
#define COGO_DO_DEFINE1(...)    COGO_DO_DEFINE2(__VA_ARGS__)
#define COGO_DO_DEFINE2(N, ...) COGO_DO_DEFINE3_##N(__VA_ARGS__)
#define COGO_DO_DEFINE3_0(NAME) void NAME##_resume(COGO_T* const cogo_this)                      // NAME: name
#define COGO_DO_DEFINE3_1(NAME) static void COGO_BLANK_##NAME##_resume(COGO_T* const cogo_this)  // NAME: static name

#define COGO_COMMA_static       ,
#define COGO_COMMA_extern       ,
#define COGO_BLANK_static
#define COGO_BLANK_extern

#define COGO_STATUS(DERIVANT) COGO_PC(&(DERIVANT)->base_cogo)

#define COGO_INIT(NAME, THIZ, ...) \
  ((NAME##_t){COGO_YIELD_INITIALIZER(NAME), __VA_ARGS__})

// Continue to run a suspended coroutine until yield or finished.
#define COGO_RESUME(DERIVANT) cogo_yield_resume(&(DERIVANT)->base_cogo)
cogo_pc_t cogo_yield_resume(cogo_yield_t* cogo_this);

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
