/*

* API
COGO_THIS
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
CO_AWAIT  (CO)  : run another coroutine until finished

COGO_INIT   (TYPE, OBJECT)
cogo_status_t
COGO_STATUS (CO)
COGO_RESUME (CO)
COGO_RUN    (CO)

CO_DECLARE(TYPE, ...){}
CO_DEFINE (TYPE)     {}

cogo_call_t    : coroutine type

*/
#ifndef COGO_CALL_H_
#define COGO_CALL_H_

#ifndef COGO_T
    #define COGO_T cogo_call_t
#endif
typedef struct cogo_call cogo_call_t;

#include "cogo_pt.h"

#ifndef COGO_SCHED_T
    #define COGO_SCHED_T cogo_call_sched_t
#endif
typedef struct cogo_call_sched cogo_call_sched_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COGO_PRE_AWAIT
    #define COGO_PRE_AWAIT(COGO_THIS, COGO_CALLEE)  // noop
#endif

#ifndef COGO_POST_AWAIT
    #define COGO_POST_AWAIT(COGO_THIS, COGO_CALLEE)  // noop
#endif

/// Implement call stack.
/// @extends cogo_pt_t
struct cogo_call {
    cogo_pt_t cogo_pt;

    // The coroutine function.
    void (*cogo_func)(COGO_T*);

    // build call stack
    COGO_T* cogo_caller;

    union {
        // resume point
        COGO_T* top;

        // associated scheduler
        COGO_SCHED_T* sched;
    } cogo_union;
};

#define COGO_CALL_INIT(COGO, FUNC)   \
    {                                \
            /*.cogo_pt=*/{0},        \
            /*.cogo_func=*/(FUNC),   \
            /*.cogo_caller=*/0,      \
            /*.a=*/{                 \
                    /*.top=*/(COGO), \
            },                       \
    }

#define COGO_CALL_OF(CALL) (CALL)
#undef COGO_PT_OF
#define COGO_PT_OF(COGO)     (&COGO_CALL_OF(COGO)->cogo_pt)
#define COGO_FUNC_OF(COGO)   (COGO_CALL_OF(COGO)->cogo_func)
#define COGO_CALLER_OF(COGO) (COGO_CALL_OF(COGO)->cogo_caller)
#define COGO_TOP_OF(COGO)    (COGO_CALL_OF(COGO)->cogo_union.top)
#define COGO_SCHED_OF(COGO)  (COGO_CALL_OF(COGO)->cogo_union.sched)

static inline int cogo_call_is_valid(cogo_call_t const* const call) {
    return call && call->cogo_func;
}

struct cogo_call_sched {
    // call stack top
    COGO_T* cogo_top;
};

#define COGO_CALL_SCHED_INIT(COGO) \
    {                              \
            /*.top=*/(COGO),       \
    }

#define COGO_CALL_SCHED_OF(CALL_SCHED) (CALL_SCHED)

/// Run another coroutine until finished.
#define COGO_AWAIT(COGO_THIS, COGO_CALLEE)                                                                                  \
    do {                                                                                                                    \
        COGO_ASSERT((COGO_THIS) == (COGO_THIS) && (COGO_THIS) && (COGO_CALLEE) == (COGO_CALLEE) && (COGO_CALLEE));          \
        COGO_PRE_AWAIT((+(COGO_THIS)), (+(COGO_CALLEE)));                                                                   \
                                                                                                                            \
        COGO_CALLER_OF(COGO_CALLEE) = (COGO_THIS);                                         /* call stack push */            \
        COGO_CALL_SCHED_OF(COGO_SCHED_OF(COGO_THIS))->cogo_top = COGO_TOP_OF(COGO_CALLEE); /* continue from resume point */ \
        COGO_DO_YIELD(COGO_THIS);                                                                                           \
                                                                                                                            \
        COGO_POST_AWAIT((+(COGO_THIS)), (+(COGO_CALLEE)));                                                                  \
    } while (0)

#define COGO_INIT(CALL, FUNC) COGO_CALL_INIT(CALL, FUNC)

/*
#undef COGO_BEGIN
#define COGO_BEGIN(COGO)                     \
    COGO_ASSERT((COGO) == (COGO) && (COGO)); \
    if (0 && (COGO) != (COGO)->cogo_union.top) {   \
        cogo_call_resume(COGO);             \
        return;                              \
    } else                                   \
        COGO_DO_BEGIN(COGO)
*/

#define COGO_RESUME(CALL)     cogo_call_resume(CALL)
cogo_pc_t cogo_call_resume(cogo_call_t* call);

#define COGO_RUN(CALL)                                  \
    do {                                                \
        COGO_ASSERT(cogo_call_is_valid(CALL));          \
        while (cogo_call_resume(CALL) != COGO_PC_END) { \
        }                                               \
    } while (0)

#define CO_AWAIT(COGO_CALLEE) COGO_AWAIT(COGO_THIS, COGO_CALLEE)

#ifdef __cplusplus
}
#endif
#endif  // COGO_CALL_H_
