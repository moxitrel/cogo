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

#include "cogo_yield.h"

#ifndef COGO_SCHED_T
    #define COGO_SCHED_T cogo_await_sched_t
#endif
typedef struct cogo_await_sched cogo_await_sched_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef COGO_PRE_AWAIT
    #define COGO_PRE_AWAIT(COGO_THIS, DERIVANT_OTHER)  // noop
#endif

#ifndef COGO_POST_AWAIT
    #define COGO_POST_AWAIT(COGO_THIS, DERIVANT_OTHER)  // noop
#endif

/// Implement call stack.
/// @extends cogo_yield_t
struct cogo_await {
    // inherit from cogo_yield_t
    cogo_yield_t base_yield;

    // build call stack
    COGO_T* caller;

    union {
        // associated scheduler
        COGO_SCHED_T* sched;

        // resume point
        COGO_T* top;
    } anon;
};

struct cogo_await_sched {
    // call stack top
    COGO_T* top;
};

#define COGO_AWAIT_INIT(TYPE, DERIVANT)              \
    {                                                \
            .base_yield = COGO_YIELD_INIT(TYPE),     \
            .anon = {.top = &(DERIVANT)->COGO_THIS}, \
    }

static inline int cogo_await_is_valid(cogo_await_t const* const cogo) {
    return cogo && cogo_yield_is_valid(&cogo->base_yield);
}

#define COGO_AWAIT_SCHED_INIT(COGO) \
    {                               \
            .top = (COGO),          \
    }

#define COGO_AWAIT_OF(AWAIT)             (AWAIT)
#define COGO_AWAIT_SCHED_OF(AWAIT_SCHED) (AWAIT_SCHED)
#undef COGO_YIELD_OF
#define COGO_YIELD_OF(COGO) (&COGO_AWAIT_OF(COGO)->base_yield)

/// Run another coroutine until finished.
#define CO_AWAIT(DERIVANT_OTHER)                                                                 \
    do {                                                                                         \
        COGO_ASSERT((DERIVANT_OTHER) == (DERIVANT_OTHER) && (DERIVANT_OTHER));                   \
        COGO_PRE_AWAIT((+COGO_THIS), (+(DERIVANT_OTHER)));                                       \
        cogo_await_await(COGO_AWAIT_OF(COGO_THIS), COGO_AWAIT_OF(&(DERIVANT_OTHER)->COGO_THIS)); \
        COGO_DO_YIELD(COGO_THIS);                                                                \
        COGO_POST_AWAIT((+COGO_THIS), (+(DERIVANT_OTHER)));                                      \
    } while (0)
void cogo_await_await(cogo_await_t* cogo, cogo_await_t* cogo_other);

#undef COGO_INIT
#define COGO_INIT(TYPE, DERIVANT) COGO_AWAIT_INIT(TYPE, DERIVANT)

// Continue to run a suspended coroutine until yield or finished.
#undef COGO_RESUME
#define COGO_RESUME(DERIVANT) cogo_await_resume(COGO_AWAIT_OF(&(DERIVANT)->COGO_THIS))
cogo_pc_t cogo_await_resume(cogo_await_t* cogo);

#define COGO_RUN(DERIVANT) cogo_await_run(COGO_AWAIT_OF(&(DERIVANT)->COGO_THIS))
void cogo_await_run(cogo_await_t* cogo);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
