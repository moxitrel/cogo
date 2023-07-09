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
#ifndef COGO_YIELD_LABEL_VALUE_H_
#define COGO_YIELD_LABEL_VALUE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef assert
#define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
#define COGO_ASSERT(...) /*noop*/
#endif

typedef intptr_t cogo_pc_t;
#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)

// implement yield
typedef struct cogo_yield {
  // start point where coroutine function continue to run after yield.
  //  0: inited
  // -1: finished
  cogo_pc_t pc;
} cogo_yield_t;

// cogo_yield_t.pc
#define COGO_PC(COGO_YIELD_V) (((cogo_yield_t *)(COGO_YIELD_V))->pc)

#define CO_STATUS_BEGIN       COGO_PC_BEGIN
#define CO_STATUS_END         COGO_PC_END
// get the current running state
static inline cogo_pc_t co_status(cogo_yield_t const *const co) {
  COGO_ASSERT(co);
  return co->pc;
}

#define COGO_BEGIN(COGO_YIELD_V)                                                                     \
  switch (COGO_PC(COGO_YIELD_V)) {                                                                   \
    case COGO_PC_BEGIN:                                                                              \
      goto cogo_begin;                                                                               \
      /* HACK: no warn unused label */                                                               \
      goto cogo_return;                                                                              \
      /* HACK: avoid clang error - indirect goto in function with no address-of-label expressions */ \
      COGO_PC(COGO_YIELD_V) = (cogo_pc_t)(&&cogo_begin);                                             \
    case COGO_PC_END:                                                                                \
      goto cogo_end;                                                                                 \
    default:                                                                                         \
      goto *(void const *)COGO_PC(COGO_YIELD_V);                                                     \
  }                                                                                                  \
  cogo_begin

#define COGO_YIELD(COGO_YIELD_V)                                                   \
  do {                                                                             \
    COGO_PC(COGO_YIELD_V) = (cogo_pc_t)(&&COGO_LABEL); /* 1. save restore point */ \
    goto cogo_end;                                     /* 2. return */             \
  COGO_LABEL:;                                         /* 3. restore point */      \
  } while (0)

#define COGO_RETURN(COGO_YIELD_V) \
  goto cogo_return /* end coroutine */

#define COGO_END(COGO_YIELD_V)              \
  cogo_return:                              \
  /**/ COGO_PC(COGO_YIELD_V) = COGO_PC_END; \
  cogo_end

// Make goto label.
// e.g. COGO_LABEL -> COGO_LABEL(__LINE__) -> cogo_yield_118
#define COGO_LABEL       COGO_LABEL1(__LINE__)
#define COGO_LABEL1(...) COGO_LABEL2(__VA_ARGS__)
#define COGO_LABEL2(N)   cogo_yield_##N

#define CO_BEGIN         COGO_BEGIN(co_this)
#define CO_END           COGO_END(co_this)
#define CO_YIELD         COGO_YIELD(co_this)
#define CO_RETURN        COGO_RETURN(co_this)

#ifdef __cplusplus
}
#endif
#endif /* COGO_YIELD_LABEL_VALUE_H_ */
