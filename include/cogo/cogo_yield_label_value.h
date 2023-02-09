/* Labels as Values (GCC Extension)

* Example
void nat_func(nat_t* co_this)
{
CO_BEGIN:

    for (co_this->i = 0; ;co_this->i++) {
        CO_YIELD;
    }

CO_END:;
}

* Internal
void nat_func(nat_t* co_this)
{
    if (co_this->pc) {          //
        goto *co_this->pc;      // CO_BEGIN:
    }                           //

    for (co_this->i = 0; ;co_this->i++) {

        pc = &&yield_11;        //
        return;                 // CO_YIELD;
    yield_11:;                  //

    }

    pc = &&yield_end;           // CO_END:
yield_end:;                     //
}

* Drawbacks
- Use GCC extension.

*/
#ifndef COGO_COGO_YIELD_IMPL_H_
#define COGO_COGO_YIELD_IMPL_H_

#include <stdint.h>

#include "macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

#define COGO_STATUS_INITED  0
#define COGO_STATUS_STOPPED -1

// yield context
typedef struct cogo_yield {
  // start point where coroutine function continue to run after yield.
  //  0: inited
  // -1: finished
  intptr_t cogo_pc;
} cogo_yield_t;

// cogo_yield_t.cogo_pc
#define COGO_PC (((cogo_yield_t *)co_this)->cogo_pc)

// get the current running state
static inline intptr_t co_status(void *co) { return ((cogo_yield_t *)co)->cogo_pc; }

#define CO_BEGIN                           \
  switch (co_status(co_this)) {            \
    case COGO_STATUS_INITED:               \
      goto cogo_enter;                     \
      /* HACK: overcome compiling error */ \
      COGO_PC = (intptr_t)(&&cogo_enter);  \
    case COGO_STATUS_STOPPED:              \
      goto cogo_exit;                      \
    default:                               \
      goto *(const void *)COGO_PC;         \
  }                                        \
  cogo_enter

#define CO_YIELD                                                    \
  do {                                                              \
    COGO_PC = (intptr_t)(&&COGO_LABEL); /* 1. save restore point */ \
    goto cogo_exit;                     /* 2. return */             \
  COGO_LABEL:;                          /* 3. restore point */      \
  } while (0)

#define CO_RETURN goto cogo_return /* end coroutine */

#define CO_END                        \
  cogo_return:                        \
  /**/ COGO_PC = COGO_STATUS_STOPPED; \
  cogo_exit

// Make goto label.
// e.g. COGO_LABEL(13)       -> cogo_yield_13
//      COGO_LABEL(__LINE__) -> cogo_yield_118
#define COGO_LABEL       COGO_LABEL1(__LINE__)
#define COGO_LABEL1(...) COGO_LABEL2(__VA_ARGS__)
#define COGO_LABEL2(N)   cogo_yield_##N

#ifdef __cplusplus
}
#endif
#endif /* COGO_COGO_YIELD_IMPL_H_ */
