/*

* API
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
co_this
CO_DECLARE(NAME, ...)
CO_DEFINE (NAME)
CO_MAKE   (NAME, ...)

cogo_await_t                          : coroutine type
cogo_await_sched_t                    : sheduler  type
CO_AWAIT      (cogo_await_t*)         : call another coroutine.
CO_SCHED_MAKE (cogo_await_t* co)      : construct a cogo_await_sched_t with co as stack top
CO_SCHED_STEP (cogo_await_sched_t*)   : run the current coroutine until yield or finished, return the next coroutine to be run.
CO_SCHED_RUN  (cogo_await_sched_t*)   : run the coroutines until all finished

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

// CO_AWAIT(cogo_await_t*): call another coroutine.
#define CO_AWAIT(CALLEE)                                                   \
  do {                                                                     \
    cogo_await_call_fast((cogo_await_t*)co_this, (cogo_await_t*)(CALLEE)); \
    CO_YIELD;                                                              \
  } while (0)
static inline void cogo_await_call_fast(cogo_await_t* const co_this, cogo_await_t* const callee) {
  COGO_ASSERT(co_this && co_this->sched && callee && !callee->caller && callee != co_this);
  // call stack push (resolve co_this->sched->stack_top != co_this)
  callee->caller = co_this->sched->stack_top;
  //callee->sched = co_this->sched->stack_top->sched;
  co_this->sched->stack_top = callee;
}
void cogo_await_call_check(cogo_await_t* co_this, cogo_await_t* callee);

static inline cogo_await_t* cogo_await_return(cogo_await_t* const co_this) {
  COGO_ASSERT(co_this && co_this->sched);
  return co_this->sched->stack_top = co_this->caller;
}

#define CO_SCHED_MAKE(CO)    ((cogo_await_sched_t){.stack_top = (cogo_await_t*)(CO)})

// run the coroutine in stack top until yield or finished.
#define CO_SCHED_STEP(SCHED) cogo_await_sched_step(SCHED)
cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* sched);

// run the coroutines until all finished
#define CO_SCHED_RUN(SCHED) cogo_await_sched_run(SCHED)
static inline void cogo_await_sched_run(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched);
  while (CO_SCHED_STEP(sched)) {
    // noop
  }
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
#endif  // COGO_COGO_AWAIT_H_
