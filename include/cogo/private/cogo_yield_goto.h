/* Copyright (c) 2018-2024 Moxi Color

Use of this source code is governed by a MIT-style license
that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

---

* Labels as Values (GCC Extension)

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
#ifndef COGO_YIELD_GOTO_H_
#define COGO_YIELD_GOTO_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef intptr_t cogo_pc_t;

// implement yield
typedef struct cogo_yield {
  // Label address where coroutine function continue to run after yield.
  //  0: inited
  // -1: finished
  cogo_pc_t private_pc;
} cogo_yield_t;

#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)
#define COGO_PC(CO)   (((cogo_yield_t*)(CO))->private_pc)

#define COGO_BEGIN(CO)                                                                            \
  switch (COGO_PC(CO)) {                                                                          \
    case COGO_PC_BEGIN:                                                                           \
      COGO_ON_BEGIN(((void const*)(CO)));                                                         \
      goto cogo_begin;                                                                            \
      /* eliminate warning of unused label */                                                     \
      goto cogo_return;                                                                           \
      /* eliminate clang error: indirect goto in function with no address-of-label expressions */ \
      COGO_PC(CO) = (cogo_pc_t)(&&cogo_begin);                                                    \
    case COGO_PC_END:                                                                             \
      goto cogo_end;                                                                              \
    default:                                                                                      \
      goto*(void const*)COGO_PC(CO);                                                              \
  }                                                                                               \
  cogo_begin

#define COGO_YIELD(CO)                                                   \
  do {                                                                   \
    COGO_ON_YIELD(((void const*)(CO)), __LINE__);                        \
    COGO_PC(CO) = (cogo_pc_t)(&&COGO_LABEL); /* 1. save restore point */ \
    goto cogo_end;                           /* 2. return */             \
  COGO_LABEL:                                /* 3. restore point */      \
    COGO_ON_RESUME(((void const*)(CO)), __LINE__);                       \
  } while (0)

#define COGO_RETURN(CO)                  \
  do {                                   \
    COGO_ON_RETURN(((void const*)(CO))); \
    goto cogo_return;                    \
  } while (0)

#define COGO_END(CO)                \
  cogo_return:                      \
  COGO_ON_END(((void const*)(CO))); \
  COGO_PC(CO) = COGO_PC_END;        \
  cogo_end

// Make goto label.
// e.g. COGO_LABEL -> COGO_LABEL(__LINE__) -> cogo_yield_118
#define COGO_LABEL       COGO_LABEL1(__LINE__)
#define COGO_LABEL1(...) COGO_LABEL2(__VA_ARGS__)
#define COGO_LABEL2(N)   cogo_yield_##N

// Invoked when coroutine begin (enter for the first time).
#ifndef COGO_ON_BEGIN
#define COGO_ON_BEGIN(CO)
#endif

// Invoked when COGO_YIELD() is called.
#ifndef COGO_ON_YIELD
#define COGO_ON_YIELD(CO, NEXT_PC)
#endif

// Invoked when coroutine resumed (continue to run).
#ifndef COGO_ON_RESUME
#define COGO_ON_RESUME(CO, PC)
#endif

// Invoked when COGO_RETURN() is called.
#ifndef COGO_ON_RETURN
#define COGO_ON_RETURN(CO)
#endif

// Invoked when coroutine end (finished).
#ifndef COGO_ON_END
#define COGO_ON_END(CO)
#endif

typedef cogo_pc_t co_status_t;
#define CO_STATUS_BEGIN COGO_PC_BEGIN
#define CO_STATUS_END   COGO_PC_END
#define CO_STATUS(CO)   ((co_status_t)COGO_PC(CO))

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_GOTO_H_
