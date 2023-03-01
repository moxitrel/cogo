/*

* API
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
co_this
co_status ()
CO_DECLARE(NAME, ...)
CO_DEFINE (NAME)
CO_MAKE   (NAME, ...)
CO_AWAIT  (cogo_await_t*)   : call another coroutine.
cogo_await_t                : coroutine type
cogo_await_sched_t          : sheduler  type
cogo_await_sched_step(cogo_await_sched_t*)  : run the current coroutine until yield or finished, return the next coroutine to be run.
cogo_await_sched_push(cogo_await_sched_t*, cogo_await_t*): add coroutine to the running queue, should be implemented by user
cogo_await_sched_pop (cogo_await_sched_t*)  : return and remove the next coroutine to be run, should be implemented by user

*/
#ifndef COGO_COGO_AWAIT_H_
#define COGO_COGO_AWAIT_H_

#include "cogo_yield.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_await cogo_await_t;              // coroutine
typedef struct cogo_await_sched cogo_await_sched_t;  // scheduler

// support call stack, concurrency
struct cogo_await {
  // inherit cogo_yield_t
  cogo_yield_t super;

  // the coroutine function
  void (*func)(void* co_this);

  // build call stack
  cogo_await_t* caller;

  // scheduler
  cogo_await_sched_t* sched;
};

// cogo_await_t scheduler
struct cogo_await_sched {
  // the current coroutine run by scheduler
  cogo_await_t* stack_top;
};

// add coroutine to the concurrent queue
// switch context if return non-zero
int cogo_await_sched_push(cogo_await_sched_t* sched, cogo_await_t* co);

// return the next coroutine to be run, and remove it from the queue
cogo_await_t* cogo_await_sched_pop(cogo_await_sched_t* sched);

// run the coroutine in stack top until yield or finished, return the next coroutine to be run.
cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* sched);

// CO_AWAIT(cogo_await_t*): call another coroutine.
// NOTE: require no loop in call chain.
#define CO_AWAIT(CO)                                               \
  do {                                                             \
    cogo_await_await((cogo_await_t*)co_this, (cogo_await_t*)(CO)); \
    CO_YIELD;                                                      \
  } while (0)
static inline void cogo_await_await(cogo_await_t* const caller, cogo_await_t* const callee) {
  COGO_ASSERT(caller);
  COGO_ASSERT(caller->sched);
  COGO_ASSERT(caller->sched->stack_top == caller);
  COGO_ASSERT(callee);

  // call stack push
  callee->caller = caller;
  //callee->sched = caller->sched;
  caller->sched->stack_top = callee;
}

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_await_t super, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.func = NAME##_func}, __VA_ARGS__})

#ifdef __cplusplus
}
#endif
#endif /* COGO_COGO_AWAIT_H_ */
