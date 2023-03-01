/*

* API
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
co_status ()
co_this
CO_DECLARE(NAME, ...)
CO_DEFINE (NAME)
CO_MAKE   (NAME, ...)
CO_AWAIT  (cogo_await_t*)   : call another coroutine.
CO_START  (cogo_await_t*)   : run a new coroutine concurrently.
cogo_await_t                : coroutine type
cogo_await_sch_t            : sheduler  type
cogo_sch_step  (cogo_await_sch_t*): run the current coroutine until yield or finished, return the next coroutine to be run.
cogo_sch_push(cogo_await_sch_t*, cogo_await_t*): add coroutine to the running queue, should be implemented by user
cogo_sch_pop (cogo_await_sch_t*)  : return and remove the next coroutine to be run, should be implemented by user

*/
#ifndef COGO_COGO_AWAIT_H_
#define COGO_COGO_AWAIT_H_

#include "cogo_yield.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_await cogo_await_t;          // coroutine
typedef struct cogo_await_sch cogo_await_sch_t;  // scheduler

// support call stack, concurrency
struct cogo_await {
  // inherit cogo_yield_t
  cogo_yield_t super;

  // the coroutine function
  void (*func)(void* co_this);

  // build call stack
  cogo_await_t* caller;

  // scheduler
  cogo_await_sch_t* sch;
};

// cogo_await_t scheduler
struct cogo_await_sch {
  // the current coroutine run by scheduler
  cogo_await_t* stack_top;
};

// add coroutine to the concurrent queue
// switch context if return non-zero
int cogo_sch_push(cogo_await_sch_t* sch, cogo_await_t* co);

// return the next coroutine to be run, and remove it from the queue
cogo_await_t* cogo_sch_pop(cogo_await_sch_t* sch);

// CO_AWAIT(cogo_await_t*): call another coroutine.
// NOTE: require no loop in call chain.
#define CO_AWAIT(CO)                                               \
  do {                                                             \
    cogo_await_await((cogo_await_t*)co_this, (cogo_await_t*)(CO)); \
    CO_YIELD;                                                      \
  } while (0)
static inline void cogo_await_await(cogo_await_t* const caller, cogo_await_t* const callee) {
  COGO_ASSERT(caller);
  COGO_ASSERT(caller->sch);
  COGO_ASSERT(caller->sch->stack_top == caller);
  COGO_ASSERT(callee);

  // call stack push
  callee->caller = caller->sch->stack_top;
  //callee->sch = caller->sch->stack_top->sch;
  caller->sch->stack_top = callee;
}

// CO_START(cogo_await_t*): add a new coroutine to the scheduler.
#define CO_START(CO)                                                              \
  do {                                                                            \
    if (cogo_sch_push(((cogo_await_t*)co_this)->sch, (cogo_await_t*)(CO)) != 0) { \
      CO_YIELD;                                                                   \
    }                                                                             \
  } while (0)

// run the coroutine in stack top until yield or finished, return the next coroutine to be run.
cogo_await_t* cogo_sch_step(cogo_await_sch_t* sch);

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_await_t cogo_await, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.func = NAME##_func}, __VA_ARGS__})

#ifdef __cplusplus
}
#endif
#endif /* COGO_COGO_AWAIT_H_ */
