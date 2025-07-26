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

#ifndef COGO_BEFORE_AWAIT
    #define COGO_BEFORE_AWAIT(COGO, COGO_OTHER)  // noop
#endif

#ifndef COGO_AFTER_AWAIT
    #define COGO_AFTER_AWAIT(COGO, COGO_OTHER)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct cogo_call {
    cogo_pt_t pt;

    // The coroutine function.
    void (*fun)(COGO_T*);

    // build call stack
    COGO_T* caller;

    union {
        // resume point
        COGO_T* top;

        // associated scheduler
        COGO_SCHED_T* sch;
    } a;
};

#define COGO_INIT(CALL, FUNC) COGO_CALL_INIT(CALL, FUNC)
#define COGO_CALL_INIT(COGO, FUNC)   \
    {                                \
            /*.pt=*/{0},             \
            /*.func=*/(FUNC),        \
            /*.caller=*/0,           \
            /*.a=*/{                 \
                    /*.top=*/(COGO), \
            },                       \
    }

#define COGO_CALL_OF(CALL) (CALL)
#undef COGO_PT_OF
#define COGO_PT_OF(COGO)     (&COGO_CALL_OF(COGO)->pt)
#define COGO_FUN_OF(COGO)    (COGO_CALL_OF(COGO)->fun)
#define COGO_CALLER_OF(COGO) (COGO_CALL_OF(COGO)->caller)
#define COGO_TOP_OF(COGO)    (COGO_CALL_OF(COGO)->a.top)
#define COGO_SCH_OF(COGO)    (COGO_CALL_OF(COGO)->a.sch)

#undef COGO_IS_VALID
#define COGO_IS_VALID(CALL)      ((CALL) == (CALL) && (CALL) && COGO_IS_VALID_CALL(CALL))
#define COGO_IS_VALID_CALL(COGO) (COGO_IS_VALID_PT(COGO) && COGO_FUN_OF(COGO) && ((COGO_TOP_OF(COGO) && COGO_STATUS(COGO) != COGO_STATUS_END) || (!COGO_TOP_OF(COGO) && COGO_STATUS(COGO) == COGO_STATUS_END)))

struct cogo_call_sched {
    // call stack top
    COGO_T* top;
};

#define COGO_SCHED_INIT(CALL) COGO_CALL_SCHED_INIT(CALL)
#define COGO_CALL_SCHED_INIT(COGO) \
    {                              \
            /*.top=*/(COGO),       \
    }

#define COGO_CALL_SCHED_OF(CALL_SCHED)  (CALL_SCHED)
#define COGO_SCHED_TOP_OF(SCHED)        (COGO_CALL_SCHED_OF(SCHED)->top)

#define COGO_SCHED_IS_VALID(CALL_SCHED) ((CALL_SCHED) == (CALL_SCHED) && (CALL_SCHED) && COGO_IS_VALID_CALL_SCHED(CALL_SCHED))
#define COGO_IS_VALID_CALL_SCHED(SCHED) (COGO_SCHED_TOP_OF(SCHED) && COGO_IS_VALID(COGO_SCHED_TOP_OF(SCHED)) || !COGO_SCHED_TOP_OF(SCHED))

/// Run another coroutine until finished.
#define CO_AWAIT(COGO_OTHER)            COGO_AWAIT(COGO_THIS, COGO_OTHER)
#define COGO_AWAIT(COGO, COGO_OTHER)                                                                     \
    do {                                                                                                 \
        COGO_ASSERT(COGO_IS_VALID(COGO) && COGO_IS_VALID(COGO_OTHER));                                   \
        COGO_BEFORE_AWAIT((+(COGO)), (+(COGO_OTHER)));                                                   \
                                                                                                         \
        COGO_CALLER_OF(COGO_OTHER) = (COGO);                            /* call stack push */            \
        COGO_SCHED_TOP_OF(COGO_SCH_OF(COGO)) = COGO_TOP_OF(COGO_OTHER); /* continue from resume point */ \
        COGO_DO_YIELD(COGO);                                                                             \
                                                                                                         \
        COGO_AFTER_AWAIT((+(COGO)), (+(COGO_OTHER)));                                                    \
    } while (0)

#define COGO_RESUME(CALL) cogo_call_resume(CALL)
cogo_call_t* cogo_call_resume(cogo_call_t* call);

#ifdef __cplusplus
}
#endif
#endif  // COGO_CALL_H_
