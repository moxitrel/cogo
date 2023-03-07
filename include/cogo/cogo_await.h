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

cogo_await_t                          : coroutine type
cogo_await_sched_t                    : sheduler  type
CO_AWAIT      (cogo_await_t*)         : call another coroutine.
CO_SCHED_STEP (cogo_await_sched_t*)   : run the current coroutine until yield or finished, return the next coroutine to be run.
CO_RUN        (cogo_await_t*)         : run the coroutines until all finished

*/
#ifndef COGO_COGO_AWAIT_H_
#define COGO_COGO_AWAIT_H_

#include "cogo_yield.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_await cogo_await_t;
typedef struct cogo_await_sched cogo_await_sched_t;

// coroutine: support call stack, concurrency
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
  // current running coroutine
  cogo_await_t* stack_top;
};

// run the coroutine in stack top until yield or finished.
cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* sched);

// run the coroutines until all finished
void cogo_await_run(cogo_await_t* co);

// CO_AWAIT(cogo_await_t*): call another coroutine.
#define CO_AWAIT(CALLEE)                                              \
  do {                                                                \
    cogo_await_call((cogo_await_t*)co_this, (cogo_await_t*)(CALLEE)); \
    CO_YIELD;                                                         \
  } while (0)
static inline void cogo_await_call(cogo_await_t* const co_this, cogo_await_t* const callee) {
  COGO_ASSERT(co_this && co_this->sched && callee && !callee->caller && callee != co_this);
  // COGO_ASSERT(co_this->sched->stack_top == co_this);
#if 0
  // check no loop in call chain
  for (cogo_await_t* co = co_this; co; co = co->caller) {
    COGO_ASSERT(callee != co);
  }
#endif

  // call stack push
  callee->caller = co_this->sched->stack_top;
  //callee->sched = co_this->sched->stack_top->sched;
  co_this->sched->stack_top = callee;
}

static inline cogo_await_t* cogo_await_return(cogo_await_t* const co_this) {
  COGO_ASSERT(co_this && co_this->sched);
  return co_this->sched->stack_top = co_this->caller;
}

#define CO_SCHED_STEP(SCHED) cogo_await_sched_step((cogo_await_sched_t*)SCHED)

#define CO_RUN(CO)           cogo_await_run((cogo_await_t*)CO)

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_await_t super, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.func = NAME##_func}, __VA_ARGS__})

#ifdef __cplusplus
}
#endif
#endif  // COGO_COGO_AWAIT_H_
