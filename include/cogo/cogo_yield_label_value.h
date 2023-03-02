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
#ifndef COGO_COGO_YIELD_LABEL_VALUE_H_
#define COGO_COGO_YIELD_LABEL_VALUE_H_

#include <stdint.h>

#include "macro_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __clang__
#define COGO_WNO_GNU_LABEL_AS_VALUE(...) CX2_WNO(clang, "-Wgnu-label-as-value", __VA_ARGS__)
#else
#define COGO_WNO_GNU_LABEL_AS_VALUE(...) __VA_ARGS__
#endif

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

#define CO_STATUS_INIT ((uintptr_t)0)
#define CO_STATUS_FINI ((uintptr_t)-1)

// yield context
typedef struct cogo_yield {
  // start point where coroutine function continue to run after yield.
  //  0: inited
  // -1: finished
  uintptr_t pc;
} cogo_yield_t;

// cogo_yield_t.pc
#define COGO_YIELD_PC (((cogo_yield_t *)co_this)->pc)

// get the current running state
static inline uintptr_t co_status(void const *const co) {
  COGO_ASSERT(co);
  return ((cogo_yield_t const *)co)->pc;
}

#define CO_BEGIN                                                                                     \
  switch (co_status(co_this)) {                                                                      \
    case CO_STATUS_INIT:                                                                             \
      goto cogo_enter;                                                                               \
      /* HACK: no warn unused label */                                                               \
      goto cogo_return;                                                                              \
      /* HACK: avoid clang error - indirect goto in function with no address-of-label expressions */ \
      COGO_YIELD_PC = COGO_WNO_GNU_LABEL_AS_VALUE((uintptr_t)(&&cogo_enter));                        \
    case CO_STATUS_FINI:                                                                             \
      goto cogo_exit;                                                                                \
    default:                                                                                         \
      goto COGO_WNO_GNU_LABEL_AS_VALUE(*(void const *)COGO_YIELD_PC);                                \
  }                                                                                                  \
  cogo_enter

#define CO_YIELD                                                                                        \
  do {                                                                                                  \
    COGO_YIELD_PC = COGO_WNO_GNU_LABEL_AS_VALUE((uintptr_t)(&&COGO_LABEL)); /* 1. save restore point */ \
    goto cogo_exit;                                                         /* 2. return */             \
  COGO_LABEL:;                                                              /* 3. restore point */      \
  } while (0)

#define CO_RETURN goto cogo_return /* fini coroutine */

#define CO_END                         \
  cogo_return:                         \
  /**/ COGO_YIELD_PC = CO_STATUS_FINI; \
  cogo_exit

// Make goto label.
// e.g. COGO_LABEL -> COGO_LABEL(__LINE__) -> cogo_yield_118
#define COGO_LABEL       COGO_LABEL1(__LINE__)
#define COGO_LABEL1(...) COGO_LABEL2(__VA_ARGS__)
#define COGO_LABEL2(N)   cogo_yield_##N

#ifdef __cplusplus
}
#endif
#endif /* COGO_COGO_YIELD_LABEL_VALUE_H_ */
