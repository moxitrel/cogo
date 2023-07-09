/*

* API
co_this
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
CO_AWAIT()            : run another coroutine until finished

NAME_t
CO_MAKE(NAME, ...)
CO_RESUME()

CO_DECLARE(NAME, ...){...}
CO_DEFINE(NAME){...}
co_status()
CO_RUN()

cogo_await_t          : coroutine type
cogo_await_sched_t    : sheduler  type

*/
#ifndef COGO_AWAIT_H_
#define COGO_AWAIT_H_

#include "cogo_yield.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_await cogo_await_t;
typedef struct cogo_await_sched cogo_await_sched_t;

// implement call stack
struct cogo_await {
  // inherit cogo_yield_t
  cogo_yield_t super;

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
  ((NAME##_t){{.super = {.func = NAME##_func}}, __VA_ARGS__})

#ifndef COGO_NO_RESUME
// continue to run a suspended coroutine until yield or finished
#undef CO_RESUME
#define CO_RESUME(CO) cogo_await_resume((cogo_await_t*)(CO))
cogo_await_t* cogo_await_resume(cogo_await_t* co);
#endif

// run the coroutines until all finished
#undef CO_RUN
#define CO_RUN(CO) cogo_await_run((cogo_await_t*)(CO))
void cogo_await_run(cogo_await_t* co);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
