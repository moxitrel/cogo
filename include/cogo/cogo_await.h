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
co_status_t
CO_STATUS()
CO_RESUME()
CO_RUN()

CO_DECLARE(NAME, ...){}
CO_DEFINE(NAME){}

cogo_await_t          : coroutine type

*/
#ifndef SRC_GITHUB_COM_MOXITREL_COGO_INCLUDE_COGO_COGO_AWAIT_H_
#define SRC_GITHUB_COM_MOXITREL_COGO_INCLUDE_COGO_COGO_AWAIT_H_

#include "cogo_yield.h"

#ifdef __cplusplus
extern "C" {
#endif

// implement call stack
typedef struct cogo_await {
  // inherit cogo_yield_t
  cogo_yield_t super;

  // build call stack
  struct cogo_await* caller;

  union {
    // call stack top / resume point
    struct cogo_await* top;

    // not used by cogo_await, declared for cogo_async
    void* sched;
  };
} cogo_await_t;

// CO_AWAIT(cogo_await_t*): call another coroutine.
#define CO_AWAIT(CALLEE)                                                                               \
  do {                                                                                                 \
    ((cogo_await_t*)co_this)->top = cogo_await_await((cogo_await_t*)co_this, (cogo_await_t*)(CALLEE)); \
    CO_YIELD;                                                                                          \
  } while (0)
cogo_await_t* cogo_await_await(cogo_await_t* thiz, cogo_await_t* callee);

#undef CO_DECLARE
#undef CO_MAKE
#undef CO_RESUME
#undef CO_RUN

#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_await_t super, __VA_ARGS__)

#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.super = {.func = NAME##_func}}, __VA_ARGS__})

// continue to run a suspended coroutine until yield or finished
#define CO_RESUME(CO) cogo_await_resume((cogo_await_t*)(CO))
co_status_t cogo_await_resume(cogo_await_t* co);

// run the coroutines until all finished
#define CO_RUN(CO) cogo_await_run((cogo_await_t*)(CO))
void cogo_await_run(cogo_await_t* co);

#ifdef __cplusplus
}
#endif
#endif  // SRC_GITHUB_COM_MOXITREL_COGO_INCLUDE_COGO_COGO_AWAIT_H_
