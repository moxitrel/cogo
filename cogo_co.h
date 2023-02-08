/*

* API
CO_BEGIN                : ...
CO_END                  : ...
CO_YIELD                : ...
CO_RETURN               : ...
CO_STATUS  (CO)         : ...
CO_THIS                 : ...
CO_DECLARE(NAME, ...)   : ...
CO_DEFINE (NAME)        : ...
CO_MAKE   (NAME, ...)   : ...

CO_AWAIT(cogo_co_t*)    : call another coroutine.
CO_START(cogo_co_t*)    : run a new coroutine concurrently.

cogo_co_t                   : coroutine type, should be inherited by user.
cogo_sch_t                  : sheduler  type, should be inherited by user.
cogo_sch_step(cogo_sch_t*)  : Run the current coroutine until yield or finished, return the next coroutine to be run.

* TODO: implemented by user
// add a coroutine to the running queue.
int cogo_sch_push(cogo_sch_t*, cogo_co_t*);

// return and remove the next coroutine to be run.
cogo_co_t* cogo_sch_pop(cogo_sch_t*);

*/
// clang-format off
#ifndef COGO_COGO_CO_H_
#define COGO_COGO_CO_H_

#include "cogo_yield.h"

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

    // scheduler
    cogo_sch_t* sch;
};

// cogo_co_t scheduler
struct cogo_sch {
    // the current coroutine run by scheduler
    cogo_co_t* stack_top;
};

// add coroutine into the concurrent queue
// switch context if return !0
int cogo_sch_push(cogo_sch_t*, cogo_co_t*);

// return the next coroutine to be run, and remove it from the queue
cogo_co_t* cogo_sch_pop(cogo_sch_t*);

// run the coroutine in stack top until yield or finished, return the next coroutine to be run.
cogo_co_t* cogo_sch_step(cogo_sch_t* sch);

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
#define CO_START(CO)                                                              \
    do {                                                                          \
        if (cogo_sch_push(((cogo_co_t*)(CO_THIS))->sch, (cogo_co_t*)(CO)) != 0) { \
            CO_YIELD;                                                             \
        }                                                                         \
    } while (0)

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
    COGO_DECLARE(NAME, cogo_co_t cogo_co, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
    ((NAME##_t){{.func = NAME##_func}, __VA_ARGS__})

#endif /* COGO_COGO_CO_H_ */
