/*

* API
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
co_this
CO_DECLARE(NAME, ...){...}
CO_DEFINE(NAME){...}
CO_MAKE(NAME, ...)
NAME_t
CO_AWAIT(cogo_await_t*) : run another coroutine until finished
CO_RESUME(cogo_await_t*): continue to run a suspended coroutine until yield or finished
CO_RUN(cogo_await_t*)   : run the coroutine and other created coroutines until all finished

co_status(cogo_yield_t*)
NAME_func
cogo_await_t                      : coroutine type
cogo_await_sched_t                : sheduler  type
cogo_await_return(cogo_await_t*)  : update call stack top to caller
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
  // the call stack top of current running coroutine
  cogo_await_t* call_top;
};

// CO_AWAIT(cogo_await_t*): call another coroutine.
#define CO_AWAIT(CALLEE)                                              \
  do {                                                                \
    cogo_await_call((cogo_await_t*)co_this, (cogo_await_t*)(CALLEE)); \
    CO_YIELD;                                                         \
  } while (0)
void cogo_await_call(cogo_await_t* co_this, cogo_await_t* callee);

// continue to run a suspended coroutine until yield or finished
#define CO_RESUME(CO) cogo_await_resume((cogo_await_t*)(CO))
cogo_await_t* cogo_await_resume(cogo_await_t* co);

// run the coroutines until all finished
#define CO_RUN(MAIN) cogo_await_run((cogo_await_t*)(MAIN))
void cogo_await_run(cogo_await_t* co_main);

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
