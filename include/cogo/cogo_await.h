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
    #define COGO_PRE_AWAIT(COGO, COGO_OTHER)  // noop
#endif

#ifndef COGO_POST_AWAIT
    #define COGO_POST_AWAIT(COGO, COGO_OTHER)  // noop
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

#undef COGO_INIT
#define COGO_INIT(AWAIT, FUNC) COGO_AWAIT_INIT(AWAIT, FUNC)
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

#undef COGO_IS_VALID
#define COGO_IS_VALID(AWAIT)      COGO_IS_AWAIT_VALID(AWAIT)
#define COGO_IS_AWAIT_VALID(COGO) ((COGO) == (COGO) && (COGO) && COGO_IS_PT_VALID(COGO) && COGO_FUNC_OF(COGO) && COGO_TOP_OF(COGO))

struct cogo_await_sched {
    // call stack top
    COGO_T* top;
};

#define COGO_SCHED_INIT(AWAIT) COGO_AWAIT_SCHED_INIT(AWAIT)
#define COGO_AWAIT_SCHED_INIT(COGO)     \
    {                                   \
            /*.top=*/COGO_TOP_OF(COGO), \
    }

#define COGO_AWAIT_SCHED_OF(AWAIT_SCHED)       (AWAIT_SCHED)
#define COGO_SCHED_TOP_OF(SCHED)               (COGO_AWAIT_SCHED_OF(SCHED)->top)

#define COGO_SCHED_IS_VALID(AWAIT_SCHED)       COGO_SCHED_IS_AWAIT_SCHED_VALID(AWAIT_SCHED)
#define COGO_SCHED_IS_AWAIT_SCHED_VALID(SCHED) ((SCHED) == (SCHED) && (SCHED) && COGO_IS_VALID(COGO_SCHED_TOP_OF(SCHED)))

/// Run another coroutine until finished.
#define CO_AWAIT(COGO_OTHER)                   COGO_AWAIT(COGO_THIS, COGO_OTHER)
#define COGO_AWAIT(COGO, COGO_OTHER)                                                                       \
    do {                                                                                                   \
        COGO_ASSERT(COGO_IS_VALID(COGO) && COGO_IS_VALID(COGO_OTHER));                                     \
        COGO_PRE_AWAIT((+(COGO)), (+(COGO_OTHER)));                                                        \
                                                                                                           \
        COGO_AWAITER_OF(COGO_OTHER) = (COGO);                             /* call stack push */            \
        COGO_SCHED_TOP_OF(COGO_SCHED_OF(COGO)) = COGO_TOP_OF(COGO_OTHER); /* continue from resume point */ \
        COGO_DO_YIELD(COGO);                                                                               \
                                                                                                           \
        COGO_POST_AWAIT((+(COGO)), (+(COGO_OTHER)));                                                       \
    } while (0)

#define COGO_RESUME(AWAIT) cogo_await_resume(AWAIT)
cogo_pc_t cogo_await_resume(cogo_await_t* await);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
