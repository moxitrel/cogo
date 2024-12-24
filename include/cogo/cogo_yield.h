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

#ifdef COGO_USE_COMPUTED_GOTO
  #include "private/cogo_pt_goto.h"
#else
  #include "private/cogo_pt_case.h"
#endif

#include "private/macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_yield cogo_yield_t;
struct cogo_yield {
  // The base type.
  cogo_pt_t base_pt;
  // The coroutine function.
  void (*func)(COGO_T* cogo_this);
};

#define COGO_YIELD_INIT(NAME, THIZ) \
  ((cogo_yield_t){.func = NAME##_func})

#undef COGO_PT_V
#define COGO_YIELD_V(COGO)      (COGO)
#define COGO_PT_V(COGO)         (&COGO_YIELD_V(COGO)->base_pt)

// typedef struct NAME NAME_t;
// struct NAME {
//  COGO_T cogo;
//  ...
// };
// void NAME_resume(NAME_t* const cogo_this)
#define COGO_COMMA_static       ,
#define COGO_COMMA_extern       ,
#define COGO_BLANK_static
#define COGO_BLANK_extern

#define COGO_DECLARE(NAME, ...) COGO_DO1_DECLARE(ZY_HAS_COMMA(COGO_COMMA_##NAME), NAME, __VA_ARGS__)
#define COGO_DO1_DECLARE(...)   COGO_DO2_DECLARE(__VA_ARGS__)
#define COGO_DO2_DECLARE(N, NAME, ...)     \
  COGO_DO3_DECLARE_##N(NAME, __VA_ARGS__); \
  COGO_DEFINE(NAME)
#define COGO_DO3_DECLARE_0(NAME, ...) COGO_STRUCT(NAME, __VA_ARGS__)               // NAME: name
#define COGO_DO3_DECLARE_1(NAME, ...) COGO_STRUCT(COGO_BLANK_##NAME, __VA_ARGS__)  // NAME: static name

#define COGO_STRUCT(NAME, ...)        COGO_DO1_STRUCT(ZY_IS_EMPTY(__VA_ARGS__), NAME, __VA_ARGS__)
#define COGO_DO1_STRUCT(...)          COGO_DO2_STRUCT(__VA_ARGS__)
#define COGO_DO2_STRUCT(N, NAME, ...) \
  typedef struct NAME NAME##_t;       \
  COGO_DO3_STRUCT_##N(NAME, __VA_ARGS__)
#define COGO_DO3_STRUCT_0(NAME, ...)      \
  struct NAME {                           \
    COGO_T cogo;                          \
    ZY_MAP1(;, ZY_IDENTITY, __VA_ARGS__); \
  }
#define COGO_DO3_STRUCT_1(NAME, ...) \
  struct NAME {                      \
    COGO_T cogo;                     \
  }

#define COGO_DEFINE(NAME)       COGO_DO1_DEFINE(ZY_HAS_COMMA(COGO_COMMA_##NAME), NAME)
#define COGO_DO1_DEFINE(...)    COGO_DO2_DEFINE(__VA_ARGS__)
#define COGO_DO2_DEFINE(N, ...) COGO_DO3_DEFINE_##N(__VA_ARGS__)
#define COGO_DO3_DEFINE_0(NAME) void NAME##_func(COGO_T* const cogo_this)                      // NAME: name
#define COGO_DO3_DEFINE_1(NAME) static void COGO_BLANK_##NAME##_func(COGO_T* const cogo_this)  // NAME: static name

#define COGO_STATUS(DERIVANT) COGO_PC(&(DERIVANT)->cogo)

#define COGO_INIT             COGO_YIELD_INIT

/// Continue to run a suspended coroutine until yield or finished.
#define COGO_RESUME(DERIVANT) cogo_yield_resume(COGO_YIELD_V(&(DERIVANT)->cogo))
cogo_pc_t cogo_yield_resume(cogo_yield_t* cogo_this);

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
