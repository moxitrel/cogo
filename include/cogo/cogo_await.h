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

#ifndef COGO_PRE_AWAIT
  #define COGO_PRE_AWAIT(COGO_THIS, COGO_OTHER)  // noop
#endif

#ifndef COGO_POST_AWAIT
  #define COGO_POST_AWAIT(COGO_THIS, COGO_OTHER)  // noop
#endif

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

#define COGO_AWAIT_INIT(NAME, DERIVANT)    \
  {                                        \
      .base_yield = COGO_YIELD_INIT(NAME), \
      .top = &(DERIVANT)->cogo_self,       \
  }

static inline int cogo_await_is_valid(cogo_await_t const* const cogo) {
  return cogo && cogo_yield_is_valid(&cogo->base_yield);
}

#define COGO_AWAIT_SCHED_INIT(COGO) \
  {                                 \
      .top = (COGO),                \
  }

#undef COGO_YIELD_OF
#define COGO_AWAIT_OF(COGO)        (COGO)
#define COGO_YIELD_OF(COGO)        (&COGO_AWAIT_OF(COGO)->base_yield)
#define COGO_AWAIT_SCHED_OF(SCHED) (SCHED)

/// Run another coroutine until finished.
#define CO_AWAIT(DERIVANT_OTHER)                                                             \
  do {                                                                                       \
    COGO_ASSERT((DERIVANT_OTHER) == (DERIVANT_OTHER));                                       \
    COGO_PRE_AWAIT((+cogo_this), (&(DERIVANT_OTHER)->cogo_self));                            \
    cogo_await_await(COGO_AWAIT_OF(cogo_this), COGO_AWAIT_OF(&(DERIVANT_OTHER)->cogo_self)); \
    COGO_DO_YIELD(cogo_this);                                                                \
    COGO_POST_AWAIT((+cogo_this), (&(DERIVANT_OTHER)->cogo_self));                           \
  } while (0)
void cogo_await_await(cogo_await_t* cogo_this, cogo_await_t* cogo1_base);

#undef COGO_INIT
#define COGO_INIT(NAME, DERIVANT) COGO_AWAIT_INIT(NAME, DERIVANT)

// Continue to run a suspended coroutine until yield or finished.
#undef COGO_RESUME
#define COGO_RESUME(DERIVANT) cogo_await_resume(COGO_AWAIT_OF(&(DERIVANT)->cogo_self))
cogo_pc_t cogo_await_resume(cogo_await_t* cogo_this);

#define COGO_RUN(DERIVANT) cogo_await_run(COGO_AWAIT_OF(&(DERIVANT)->cogo_self))
void cogo_await_run(cogo_await_t* cogo_this);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
