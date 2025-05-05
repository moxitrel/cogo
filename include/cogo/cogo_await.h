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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COGO_PRE_AWAIT
    #define COGO_PRE_AWAIT(COGO, COGO_AWAITEE)  // noop
#endif

#ifndef COGO_POST_AWAIT
    #define COGO_POST_AWAIT(COGO, COGO_AWAITEE)  // noop
#endif

/// Implement call stack.
/// @extends cogo_yield_t
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
    } anon;
};

struct cogo_await_sched {
    // call stack top
    COGO_T* top;
};

#define COGO_AWAIT_INIT(FUNC, COGO)  \
    {                                \
            /*.pt=*/{0},             \
            /*.func=*/(FUNC),        \
            /*.awaiter=*/0,          \
            /*.anon=*/{              \
                    /*.top=*/(COGO), \
            },                       \
    }

static inline int cogo_await_is_valid(cogo_await_t const* const await) {
    return await && await->func;
}

#define COGO_AWAIT_SCHED_INIT(COGO) \
    {                               \
            /*.top=*/(COGO),        \
    }

#define COGO_AWAIT_OF(AWAIT)             (AWAIT)
#define COGO_AWAIT_SCHED_OF(AWAIT_SCHED) (AWAIT_SCHED)
#undef COGO_PT_OF
#define COGO_PT_OF(COGO)       (&COGO_AWAIT_OF(COGO)->pt)

/// Run another coroutine until finished.
#define CO_AWAIT(COGO_AWAITEE) COGO_AWAIT(COGO_THIS, COGO_AWAITEE)
#define COGO_AWAIT(COGO, COGO_AWAITEE)                                                                 \
    do {                                                                                               \
        COGO_ASSERT((COGO) == (COGO) && (COGO) && (COGO_AWAITEE) == (COGO_AWAITEE) && (COGO_AWAITEE)); \
        COGO_PRE_AWAIT((+(COGO)), (+(COGO_AWAITEE)));                                                  \
        cogo_await_await(COGO_AWAIT_OF(COGO), COGO_AWAIT_OF(COGO_AWAITEE));                            \
        COGO_DO_YIELD(COGO);                                                                           \
        COGO_POST_AWAIT((+(COGO)), (+(COGO_AWAITEE)));                                                 \
    } while (0)
void cogo_await_await(cogo_await_t* await, cogo_await_t* awaitee);

#undef COGO_INIT
#define COGO_INIT(FUNC, AWAIT) COGO_AWAIT_INIT(FUNC, AWAIT)

/*
#undef COGO_BEGIN
#define COGO_BEGIN(COGO)                     \
    COGO_ASSERT((COGO) == (COGO) && (COGO)); \
    if (0 && (COGO) != (COGO)->anon.top) {   \
        cogo_await_resume(COGO);             \
        return;                              \
    } else                                   \
        COGO_DO_BEGIN(COGO)
*/

#define COGO_RESUME(AWAIT)     cogo_await_resume(AWAIT)
cogo_pc_t cogo_await_resume(cogo_await_t* await);

#define COGO_RUN(AWAIT) cogo_await_run(AWAIT)
void cogo_await_run(cogo_await_t* await);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
