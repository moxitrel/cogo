/*

* API
cogo_this
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
CO_AWAIT  (CO)  : run another coroutine until finished

COGO_INIT   (CO, NAME, ...)
cogo_status_t
COGO_STATUS (CO)
COGO_RESUME (CO)
COGO_RUN    (CO)

CO_DECLARE(NAME, ...){}
CO_DEFINE (NAME)     {}

cogo_await_t    : coroutine type

*/
#ifndef COGO_AWAIT_H_
#define COGO_AWAIT_H_

#ifndef COGO_T
#define COGO_T cogo_await_t
typedef struct cogo_await cogo_await_t;
#endif

#ifndef COGO_SCHED_T
#define COGO_SCHED_T cogo_await_sched_t
typedef struct cogo_await_sched cogo_await_sched_t;
#endif

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

  // build call stack
  COGO_T* caller;

  union {
    // associated scheduler
    COGO_SCHED_T* sched;

    // resume point
    COGO_T* top;
  };
};

struct cogo_await_sched {
  // call stack top
  COGO_T* top;
};

#undef COGO_YIELD_V
#define COGO_AWAIT_V(AWAIT) (AWAIT)
#define COGO_YIELD_V(AWAIT) (&COGO_AWAIT_V(AWAIT)->base_yield)

/// Run another coroutine until finished.
#define CO_AWAIT(COGO)                                                      \
  do {                                                                      \
    cogo_await_await(COGO_AWAIT_V(cogo_this), COGO_AWAIT_V(&(COGO)->cogo)); \
    CO_YIELD;                                                               \
  } while (0)
void cogo_await_await(cogo_await_t* cogo_this, cogo_await_t* cogo);

#undef COGO_INIT
#define COGO_INIT(NAME, THIZ, ...)                 \
  ((NAME##_t){                                     \
      /* .cogo = */ {                              \
          .base_yield = {.resume = NAME##_resume}, \
          .top = &(THIZ)->cogo,                    \
      },                                           \
      __VA_ARGS__})

// Continue to run a suspended coroutine until yield or finished.
#undef COGO_RESUME
#define COGO_RESUME(COGO) cogo_await_resume(&(COGO)->cogo)
cogo_pc_t cogo_await_resume(cogo_await_t* cogo);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
