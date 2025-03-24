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

CO_DECLARE(TYPE, ...){} : declare a coroutine.
  FUNC_t                : the declared coroutine type.
  FUNC_resume           : the declared coroutine function.
COGO_DEFINE (TYPE)   {} : define a declared coroutine which is not defined.

*/
#ifndef COGO_YIELD_H_
#define COGO_YIELD_H_

#include "private/macro_utils.h"

#ifndef COGO_T
    #define COGO_T cogo_yield_t
#endif
typedef struct cogo_yield cogo_yield_t;

#ifdef COGO_USE_COMPUTED_GOTO
    #include "private/cogo_pt_goto.h"
#else
    #include "private/cogo_pt_case.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// Implement yield.
/// @extends cogo_pt_t
struct cogo_yield {
    cogo_pt_t base_pt;

    // The coroutine function.
    void (*func)(COGO_T* COGO_THIS);
};

#define COGO_YIELD_INIT(TYPE)    \
    {                            \
            .func = TYPE##_func, \
    }

static inline int cogo_yield_is_valid(cogo_yield_t const* const cogo) {
    return cogo && cogo->func;
}

#define COGO_YIELD_OF(YIELD) (YIELD)
#undef COGO_PT_OF
#define COGO_PT_OF(COGO)                (&COGO_YIELD_OF(COGO)->base_pt)

/// @hideinitializer Declare the coroutine.
/// @param TYPE The coroutine name.
//
// typedef struct TYPE TYPE;
// struct TYPE {
//  COGO_T COGO_THIS;
//  ...
// };
// void NAME_func(TYPE* const COGO_THIS)
#define COGO_DECLARE(...)               COGO_DO1_DECLARE(ZY_HAS_COMMA(__VA_ARGS__), __VA_ARGS__)
#define COGO_DO1_DECLARE(...)           COGO_DO2_DECLARE(__VA_ARGS__)
#define COGO_DO2_DECLARE(N, ...)        COGO_DO3_DECLARE_##N(__VA_ARGS__)
#define COGO_DO3_DECLARE_0(TYPE)        COGO_DO4_DECLARE_0(TYPE)
#define COGO_DO3_DECLARE_1(TYPE, ...)   COGO_DO4_DECLARE_1(TYPE, __VA_ARGS__)

#define COGO_DO4_DECLARE_0(TYPE)        COGO_DO5_DECLARE_0(ZY_HAS_COMMA(COGO_COMMA_##TYPE), TYPE)
#define COGO_DO5_DECLARE_0(...)         COGO_DO6_DECLARE_0(__VA_ARGS__)
#define COGO_DO6_DECLARE_0(N, TYPE)     COGO_DO7_DECLARE_0_##N(TYPE)
#define COGO_DO7_DECLARE_0_0(TYPE)      COGO_STRUCT(TYPE)
#define COGO_DO7_DECLARE_0_1(TYPE, ...) COGO_STRUCT(COGO_COMMA_##TYPE, __VA_ARGS__)

#define COGO_DO4_DECLARE_1(...)         COGO_DO4_DECLARE_1(__VA_ARGS__)

#define COGO_DECLARE(TYPE, ...)         COGO_DO1_DECLARE(ZY_HAS_COMMA(COGO_COMMA_##TYPE), TYPE, __VA_ARGS__)
#define COGO_DO1_DECLARE(...)           COGO_DO2_DECLARE(__VA_ARGS__)
#define COGO_DO2_DECLARE(N, TYPE, ...)       \
    COGO_DO3_DECLARE_##N(TYPE, __VA_ARGS__); \
    COGO_DEFINE(TYPE)
#define COGO_DO3_DECLARE_0(TYPE, ...) COGO_STRUCT(TYPE, __VA_ARGS__)               // TYPE: name
#define COGO_DO3_DECLARE_1(TYPE, ...) COGO_STRUCT(COGO_BLANK_##TYPE, __VA_ARGS__)  // TYPE: static name

#define COGO_STRUCT(...)              COGO_DO1_STRUCT(ZY_HAS_COMMA(__VA_ARGS__), __VA_ARGS__)
#define COGO_DO1_STRUCT(...)          COGO_DO2_STRUCT(__VA_ARGS__)
#define COGO_DO2_STRUCT(N, ...)       COGO_DO3_STRUCT_##N(__VA_ARGS__)
#define COGO_DO3_STRUCT_0(TYPE) \
    typedef struct TYPE TYPE;   \
    struct TYPE {               \
        COGO_T COGO_THIS;       \
    }
#define COGO_DO3_STRUCT_1(TYPE, ...)          \
    typedef struct TYPE TYPE;                 \
    struct TYPE {                             \
        COGO_T COGO_THIS;                     \
        ZY_MAP1(;, ZY_IDENTITY, __VA_ARGS__); \
    }

#define COGO_COMMA_static ,
#define COGO_COMMA_extern ,
#define COGO_BLANK_static
#define COGO_BLANK_extern

/// @hideinitializer Define the coroutine.
/// @param[in] TYPE The coroutine name.
/// @pre `TYPE` must be the same identifier that is passed as the first argument to `COGO_DECLARE(TYPE, ...)`.
#define COGO_DEFINE(TYPE)         COGO_DO1_DEFINE(ZY_HAS_COMMA(COGO_COMMA_##TYPE), TYPE)
#define COGO_DO1_DEFINE(...)      COGO_DO2_DEFINE(__VA_ARGS__)
#define COGO_DO2_DEFINE(N, ...)   COGO_DO3_DEFINE_##N(__VA_ARGS__)
#define COGO_DO3_DEFINE_0(TYPE)   void TYPE##_func(COGO_T* const COGO_THIS)                      // TYPE: name
#define COGO_DO3_DEFINE_1(TYPE)   static void COGO_BLANK_##TYPE##_func(COGO_T* const COGO_THIS)  // TYPE: static name

/// @hideinitializer Get the current running status of the coroutine.
/// @param[in] DERIVANT
/// @pre `DERIVANT != NULL`.
#define COGO_STATUS(DERIVANT)     COGO_PC(&(DERIVANT)->COGO_THIS)

/// @hideinitializer The initializer of `COGO_T` type.
/// @param[in] TYPE The coroutine name.
/// @param[in] DERIVANT The coroutine object.
/// @pre `TYPE` must be the same identifier that is passed as the first argument to `COGO_DECLARE(TYPE, ...)`.
#define COGO_INIT(TYPE, DERIVANT) COGO_YIELD_INIT(TYPE)

/// @hideinitializer Begin to run an initialized coroutine, or continue to run a suspended coroutine, until yield or finish.
/// @param[in] DERIVANT A coroutine object pointer.
/// @pre `DERIVANT != NULL`.
#define COGO_RESUME(DERIVANT)     cogo_yield_resume(COGO_YIELD_OF(&(DERIVANT)->COGO_THIS))
cogo_pc_t cogo_yield_resume(cogo_yield_t* cogo);

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_H_
