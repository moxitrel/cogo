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
    #define COGO_BEFORE_AWAIT(COGO, COGO_CALLEE)  // noop
#endif

#ifndef COGO_AFTER_AWAIT
    #define COGO_AFTER_AWAIT(COGO, COGO_CALLEE)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct cogo_call {
    cogo_pt_t pt;

    // The coroutine function.
    void (*func)(COGO_T*);

    // build call stack
    COGO_T* caller;

    union {
        // resume point
        COGO_T* top;

        // associated scheduler
        COGO_SCHED_T* sched;
    } a;
};

#define COGO_CALL_INIT(COGO, FUNC)   \
    {                                \
            /*.pt=*/{0},             \
            /*.func=*/(FUNC),        \
            /*.caller=*/0,           \
            /*.a=*/{                 \
                    /*.top=*/(COGO), \
            },                       \
    }
#define COGO_CALL_PT(CALL)       (&(CALL)->pt)
#define COGO_CALL_FUNC(CALL)     ((CALL)->func)
#define COGO_CALL_CALLER(CALL)   ((CALL)->caller)
#define COGO_CALL_TOP(CALL)      ((CALL)->a.top)
#define COGO_CALL_SCHED(CALL)    ((CALL)->a.sched)
#define COGO_CALL_IS_VALID(CALL) (COGO_PT_IS_VALID(COGO_CALL_PT(CALL)) && COGO_CALL_FUNC(CALL))

#undef COGO_PT_OF
#undef COGO_IS_VALID
#define COGO_INIT(COGO, FUNC) COGO_CALL_INIT(COGO, FUNC)
#define COGO_CALL_OF(COGO)    (COGO)
#define COGO_PT_OF(COGO)      COGO_CALL_PT(COGO_CALL_OF(COGO))
#define COGO_FUNC_OF(COGO)    COGO_CALL_FUNC(COGO_CALL_OF(COGO))
#define COGO_CALLER_OF(COGO)  COGO_CALL_CALLER(COGO_CALL_OF(COGO))
#define COGO_TOP_OF(COGO)     COGO_CALL_TOP(COGO_CALL_OF(COGO))
#define COGO_SCHED_OF(COGO)   COGO_CALL_SCHED(COGO_CALL_OF(COGO))
#define COGO_IS_VALID(COGO)   ((COGO) == (COGO) && (COGO) && COGO_CALL_IS_VALID(COGO_CALL_OF(COGO)))

struct cogo_call_sched {
    // call stack top
    COGO_T* top;
};

#define COGO_CALLSCHED_INIT(COGO) \
    {                             \
            /*.top=*/(COGO),      \
    }
#define COGO_CALLSCHED_TOP(CALLSCHED)      ((CALLSCHED)->top)
#define COGO_CALLSCHED_IS_VALID(CALLSCHED) ((COGO_CALLSCHED_TOP(CALLSCHED) && COGO_IS_VALID(COGO_CALLSCHED_TOP(CALLSCHED))) || !COGO_CALLSCHED_TOP(CALLSCHED))

#define COGO_SCHED_INIT(COGO)              COGO_CALLSCHED_INIT(COGO)
#define COGO_CALLSCHED_OF(SCHED)           (SCHED)
#define COGO_SCHED_TOP_OF(SCHED)           COGO_CALLSCHED_TOP(COGO_CALLSCHED_OF(SCHED))

#define COGO_SCHED_IS_VALID(SCHED)         ((SCHED) == (SCHED) && (SCHED) && COGO_CALLSCHED_IS_VALID(COGO_CALLSCHED_OF(SCHED)))

/// Run another coroutine until finished.
#define COGO_AWAIT(COGO, COGO_CALLEE)                                                                       \
    do {                                                                                                    \
        COGO_ASSERT(COGO_IS_VALID(COGO) && COGO_IS_VALID(COGO_CALLEE));                                     \
        COGO_BEFORE_AWAIT((+(COGO)), (+(COGO_CALLEE)));                                                     \
                                                                                                            \
        COGO_CALLER_OF(COGO_CALLEE) = (COGO);                              /* call stack push */            \
        COGO_SCHED_TOP_OF(COGO_SCHED_OF(COGO)) = COGO_TOP_OF(COGO_CALLEE); /* continue from resume point */ \
        COGO_DO_YIELD(COGO);                                                                                \
                                                                                                            \
        COGO_AFTER_AWAIT((+(COGO)), (+(COGO_CALLEE)));                                                      \
    } while (0)

#define COGO_RESUME(COGO) cogo_call_resume(COGO)
const cogo_call_t* cogo_call_resume(cogo_call_t* cogo);

#define CO_AWAIT(COGO_CALLEE) COGO_AWAIT(COGO_THIS, COGO_CALLEE)

#ifdef __cplusplus
}
#endif
#endif  // COGO_CALL_H_
