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
    if (co_this->status) {          //
        goto *co_this->status;      // CO_BEGIN:
    }                           //

    for (co_this->i = 0; ;co_this->i++) {

        status = &&yield_11;        //
        return;                 // CO_YIELD;
    yield_11:;                  //

    }

    status = &&yield_end;           // CO_END:
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

typedef intptr_t co_status_t;
#define CO_STATUS_BEGIN 0
#define CO_STATUS_END   (-1)

// implement yield
typedef struct cogo_yield {
  // resume point where coroutine function continue to run after yield.
  //  0: inited
  // -1: finished
  co_status_t status;

  // the coroutine function
  void (*resume)(void* co_this);
} cogo_yield_t;

// get the current running state
#define CO_STATUS(CO) (((cogo_yield_t*)(CO))->status)

#define COGO_BEGIN(CO)                                                                               \
  switch (CO_STATUS(CO)) {                                                                           \
    case CO_STATUS_BEGIN:                                                                            \
      goto cogo_begin;                                                                               \
      goto cogo_return; /* HACK: no warn unused label */                                             \
      /* HACK: avoid clang error - indirect goto in function with no address-of-label expressions */ \
      CO_STATUS(CO) = (co_status_t)(&&cogo_begin);                                                   \
    case CO_STATUS_END:                                                                              \
      goto cogo_end;                                                                                 \
    default:                                                                                         \
      goto*(void*)CO_STATUS(CO);                                                                     \
  }                                                                                                  \
  cogo_begin

#define COGO_YIELD(CO)                                                       \
  do {                                                                       \
    CO_STATUS(CO) = (co_status_t)(&&COGO_LABEL); /* 1. save restore point */ \
    goto cogo_end;                               /* 2. return */             \
  COGO_LABEL:;                                   /* 3. restore point */      \
  } while (0)

#define COGO_RETURN(CO) \
  goto cogo_return /* end coroutine */

#define COGO_END(CO)                  \
  cogo_return:                        \
  /**/ CO_STATUS(CO) = CO_STATUS_END; \
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
#endif  // COGO_YIELD_LABEL_VALUE_H_
