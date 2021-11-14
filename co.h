/*

* API
CO_BEGIN                : ...
CO_END                  : ...
CO_YIELD                : ...
CO_RETURN               : ...
CO_STATE  (CO)          : ...
CO_THIS                 : ...
CO_DECLARE(NAME, ...)   : ...
CO_DEFINE (NAME)        : ...
CO_MAKE   (NAME, ...)   : ...

CO_AWAIT(cogo_co_t*)    : call another coroutine.
CO_START(cogo_co_t*)    : run a new coroutine concurrently.

cogo_co_t                   : coroutine type, should be inherited by user.
cogo_sch_t                  : sheduler  type, should be inherited by user.
cogo_sch_step(cogo_sch_t*)  : Run the current coroutine until yield or finished, return the next coroutine to be run.

// TODO: add a coroutine to the running queue.
inline int cogo_sch_add(cogo_sch_t*, cogo_co_t*);

// TODO: remove and return the next coroutine to be run.
inline cogo_co_t* cogo_sch_rm(cogo_sch_t*);


*/
#ifndef MOXITREL_COGO_CO_H_
#define MOXITREL_COGO_CO_H_

#include "yield.h"

typedef struct cogo_co cogo_co_t;    // coroutine
typedef struct cogo_sch cogo_sch_t;  // scheduler

// support call stack, concurrency
struct cogo_co {
    // inherit cogo_yield_t
    cogo_yield_t cogo_yield;

    // the coroutine function
    void (*func)(void*);

    // build call stack
    cogo_co_t* caller;

    // scheduler, updated by cogo_sch_step()
    cogo_sch_t* sch;
};

// cogo_co_t scheduler
struct cogo_sch {
    // the coroutine run by scheduler
    cogo_co_t* stack_top;
};

// add coroutine into the concurrent queue
// switch context if return !0
inline int cogo_sch_add(cogo_sch_t*, cogo_co_t*);

// remove and rethrn the next coroutine to be run
inline cogo_co_t* cogo_sch_rm(cogo_sch_t*);

//
// cogo_co_t
//

// CO_AWAIT(cogo_co_t*): call another coroutine.
// NOTE: require no loop in call chain.
#define CO_AWAIT(CO)                                            \
    do {                                                        \
        cogo_co_await((cogo_co_t*)(CO_THIS), (cogo_co_t*)(CO)); \
        CO_YIELD;                                               \
    } while (0)
static inline void cogo_co_await(cogo_co_t* thiz, cogo_co_t* callee) {
    COGO_ASSERT(thiz);
    COGO_ASSERT(thiz->sch);
    COGO_ASSERT(thiz->sch->stack_top == thiz);
    COGO_ASSERT(callee);

    // call stack push
    callee->caller = thiz->sch->stack_top;
    //callee->sch = thiz->sch->stack_top->sch;
    thiz->sch->stack_top = callee;
}

// CO_START(cogo_co_t*): add a new coroutine to the scheduler.
#define CO_START(CO)                                                             \
    do {                                                                         \
        if (cogo_sch_add(((cogo_co_t*)(CO_THIS))->sch, (cogo_co_t*)(CO)) != 0) { \
            CO_YIELD;                                                            \
        }                                                                        \
    } while (0)

//
// cogo_sch_t
//

// run the coroutine in stack top until yield or finished, return the next coroutine to be run.
inline cogo_co_t* cogo_sch_step(cogo_sch_t* sch) {
    COGO_ASSERT(sch);
    while (sch->stack_top) {
        sch->stack_top->sch = sch;
        sch->stack_top->func(sch->stack_top);
        if (!sch->stack_top) {
            // blocked
            break;
        }
        switch (CO_STATE(sch->stack_top)) {
        case 0:  // await
            continue;
        case -1u:  // return
            sch->stack_top = sch->stack_top->caller;
            continue;
        default:  // yield
            cogo_sch_add(sch, sch->stack_top);
            break;
        }
        break;
    }
    return sch->stack_top = cogo_sch_rm(sch);
}

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
    COGO_DECLARE(NAME, cogo_co_t cogo_co, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
    ((NAME){{.func = NAME##_func}, __VA_ARGS__})

#endif  // MOXITREL_COGO_CO_H_
