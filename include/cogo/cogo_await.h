/*

* API
co_this
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END
CO_AWAIT()            : run another coroutine until finished

CO_DECLARE(NAME, ...){...}
CO_DEFINE(NAME){...}
CO_MAKE(NAME, ...)
NAME_t
CO_RESUME()           : continue to run a suspended coroutine until yield or finished
CO_RUN()              : run the coroutine and all other created coroutines until finished

co_status()
cogo_await_t          : coroutine type
cogo_await_sched_t    : sheduler  type

*/
#ifndef COGO_AWAIT_H_
#define COGO_AWAIT_H_

#include "_private/cogo_yield.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_await cogo_await_t;
typedef struct cogo_await_sched cogo_await_sched_t;

// implement call stack
struct cogo_await {
  // inherit cogo_yield_t
  cogo_yield_t super;

  // the coroutine function
  void (*func)(void* co_this);

  // build call stack
  cogo_await_t* caller;

  union {
    cogo_await_sched_t* sched;

    // resume entry
    cogo_await_t* entry;
  };
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

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_await_t super, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.func = NAME##_func}, __VA_ARGS__})

#ifdef COGO_USE_RESUME
// continue to run a suspended coroutine until yield or finished
#define CO_RESUME(CO) cogo_await_resume((cogo_await_t*)(CO))
cogo_await_t* cogo_await_resume(cogo_await_t* co);
#endif

// run the coroutines until all finished
#define CO_RUN(CO) cogo_await_run((cogo_await_t*)(CO))
void cogo_await_run(cogo_await_t* co_main);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
