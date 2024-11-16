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

#ifndef COGO_ON_AWAITING
#define COGO_ON_AWAITING(COGO)  // noop
#endif

#ifndef COGO_ON_AWAITED
#define COGO_ON_AWAITED(COGO)  // noop
#endif

#ifndef COGO_MALLOC
#include <stdlib.h>
#define COGO_MALLOC(SIZE) malloc(SIZE)
#endif

#ifndef COGO_FREE
#include <stdlib.h>
#define COGO_FREE(PTR) free(PTR)
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

#define COGO_AWAIT_INITIALIZER(NAME, THIZ)      \
  {                                             \
    .base_yield = COGO_YIELD_INITIALIZER(NAME), \
    .top = &(THIZ)->base_cogo,                  \
  }

#undef COGO_YIELD_V
#define COGO_AWAIT_V(AWAIT) (AWAIT)
#define COGO_YIELD_V(COGO)  (&COGO_AWAIT_V(COGO)->base_yield)

/// Run another coroutine until finished.
#define CO_AWAIT(DERIVANT1)                                                           \
  do {                                                                                \
    COGO_ON_AWAITING(&*cogo_this);                                                    \
    cogo_await_await(COGO_AWAIT_V(cogo_this), COGO_AWAIT_V(&(DERIVANT1)->base_cogo)); \
    CO_YIELD;                                                                         \
    COGO_ON_AWAITED(&*cogo_this);                                                     \
  } while (0)
void cogo_await_await(cogo_await_t* cogo_this, cogo_await_t* cogo1_base);

#undef COGO_INIT
#define COGO_INIT(NAME, THIZ, ...) \
  ((NAME##_t){COGO_AWAIT_INITIALIZER(NAME, THIZ), __VA_ARGS__})

#define CO_CALL(NAME, ...)                       \
  NAME##_t* _cogo = COGO_MALLOC(sizeof(*_cogo)); \
  *_cogo = COGO_INIT(NAME, _cogo, __VA_ARGS__);  \
  CO_AWAIT(_cogo);                               \
  *(NAME##_t*)_cogo;                             \
  COGO_FREE(_cogo);                              \
  _cogo = NULL;

// Continue to run a suspended coroutine until yield or finished.
#undef COGO_RESUME
#define COGO_RESUME(DERIVANT) cogo_await_resume(&(DERIVANT)->base_cogo)
cogo_pc_t cogo_await_resume(cogo_await_t* cogo_this);

#ifdef __cplusplus
}
#endif
#endif  // COGO_AWAIT_H_
