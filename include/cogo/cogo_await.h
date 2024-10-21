/*

* API
cogo_this
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
CO_AWAIT  (CO)  : run another coroutine until finished

CO_INIT   (CO, NAME, ...)
cogo_status_t
COGO_STATUS (CO)
CO_RESUME (CO)
CO_RUN    (CO)

CO_DECLARE(NAME, ...){}
CO_DEFINE (NAME)     {}

cogo_await_t    : coroutine type

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
  // inherit from cogo_yield_t
  cogo_yield_t base_yield;

  // the coroutine function
  void (*resume)(void* cogo_this);

  // build call stack
  cogo_await_t* caller;

  union {
    // associated scheduler
    cogo_await_sched_t* sched;

    // resume point
    cogo_await_t* top;
  };
};

struct cogo_await_sched {
  // call stack top
  cogo_await_t* top;
};

/// Run another coroutine until finished.
#define CO_AWAIT(/*cogo_await_t**/ CO)                               \
  do {                                                               \
    cogo_await_await((cogo_await_t*)cogo_this, (cogo_await_t*)(CO)); \
    CO_YIELD;                                                        \
  } while (0)
void cogo_await_await(cogo_await_t* thiz, cogo_await_t* co);

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_await_t base_await, __VA_ARGS__)

#define CO_INIT(NAME, THIZ, ...) \
  ((NAME##_t){{.resume = NAME##_resume, .top = (cogo_await_t*)(THIZ)}, __VA_ARGS__})

// continue to run a suspended coroutine until yield or finished
#define CO_RESUME(CO) cogo_await_resume((cogo_await_t*)(CO))
cogo_status_t cogo_await_resume(cogo_await_t* co);

// run the coroutines until all finished
#define CO_RUN(CO) cogo_await_run((cogo_await_t*)(CO))
void cogo_await_run(cogo_await_t* co);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
