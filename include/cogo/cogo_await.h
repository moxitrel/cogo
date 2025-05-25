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

cogo_await_t    : coroutine type

*/
#ifndef COGO_AWAIT_H_
#define COGO_AWAIT_H_

#ifndef COGO_T
    #define COGO_T cogo_await_t
#endif
typedef struct cogo_await cogo_await_t;

#include "cogo_pt.h"

#ifndef COGO_SCHED_T
    #define COGO_SCHED_T cogo_await_sched_t
#endif
typedef struct cogo_await_sched cogo_await_sched_t;

#ifndef COGO_PRE_AWAIT
    #define COGO_PRE_AWAIT(COGO, COGO_AWAITEE)  // noop
#endif

#ifndef COGO_POST_AWAIT
    #define COGO_POST_AWAIT(COGO, COGO_AWAITEE)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// Implement call stack.
/// @extends cogo_pt_t
struct cogo_await {
    cogo_pt_t pt;

    // The coroutine function.
    void (*func)(COGO_T*);

    // build call stack
    COGO_T* awaiter;

    union {
        // resume point
        COGO_T* top;

        // associated scheduler
        COGO_SCHED_T* sched;
    } a;
};

#define COGO_AWAIT_INIT(COGO, FUNC)  \
    {                                \
            /*.pt=*/COGO_PT_INIT(),  \
            /*.func=*/(FUNC),        \
            /*.awaiter=*/0,          \
            /*.a=*/{                 \
                    /*.top=*/(COGO), \
            },                       \
    }

#define COGO_AWAIT_OF(AWAIT) (AWAIT)
#undef COGO_PT_OF
#define COGO_PT_OF(COGO)      (&COGO_AWAIT_OF(COGO)->pt)
#define COGO_FUNC_OF(COGO)    (COGO_AWAIT_OF(COGO)->func)
#define COGO_AWAITER_OF(COGO) (COGO_AWAIT_OF(COGO)->awaiter)
#define COGO_TOP_OF(COGO)     (COGO_AWAIT_OF(COGO)->a.top)
#define COGO_SCHED_OF(COGO)   (COGO_AWAIT_OF(COGO)->a.sched)

struct cogo_await_sched {
    // call stack top
    COGO_T* top;
};

#define COGO_AWAIT_SCHED_INIT(COGO)          \
    {                                        \
            /*.cogo_top=*/COGO_TOP_OF(COGO), \
    }

#define COGO_AWAIT_SCHED_OF(AWAIT_SCHED) (AWAIT_SCHED)
#define COGO_SCHED_TOP_OF(SCHED)         (COGO_AWAIT_SCHED_OF(SCHED)->top)

static inline int cogo_await_is_valid(cogo_await_t const* const await) {
    return await && await->func;
}

/// Run another coroutine until finished.
#define COGO_AWAIT(COGO, COGO_AWAITEE)                                                                       \
    do {                                                                                                     \
        COGO_ASSERT((COGO) == (COGO) && (COGO) && (COGO_AWAITEE) == (COGO_AWAITEE) && (COGO_AWAITEE));       \
        COGO_PRE_AWAIT((+(COGO)), (+(COGO_AWAITEE)));                                                        \
                                                                                                             \
        COGO_AWAITER_OF(COGO_AWAITEE) = (COGO);                             /* call stack push */            \
        COGO_SCHED_TOP_OF(COGO_SCHED_OF(COGO)) = COGO_TOP_OF(COGO_AWAITEE); /* continue from resume point */ \
        COGO_DO_YIELD(COGO);                                                                                 \
                                                                                                             \
        COGO_POST_AWAIT((+(COGO)), (+(COGO_AWAITEE)));                                                       \
    } while (0)

#undef COGO_INIT
#define COGO_INIT(AWAIT, FUNC) COGO_AWAIT_INIT(AWAIT, FUNC)
#define COGO_SCHED_INIT(AWAIT) COGO_AWAIT_SCHED_INIT(AWAIT)

/*
#undef COGO_BEGIN
#define COGO_BEGIN(COGO)                     \
    COGO_ASSERT((COGO) == (COGO) && (COGO)); \
    if (0 && (COGO) != (COGO)->a.top) {   \
        cogo_await_resume(COGO);             \
        return;                              \
    } else                                   \
        COGO_DO_BEGIN(COGO)
*/

#define COGO_RESUME(AWAIT)     cogo_await_resume(AWAIT)
cogo_pc_t cogo_await_resume(cogo_await_t* await);

#define CO_AWAIT(COGO_AWAITEE) COGO_AWAIT(COGO_THIS, COGO_AWAITEE)

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
