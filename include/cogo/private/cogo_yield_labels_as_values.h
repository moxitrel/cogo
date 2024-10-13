/*
MIT License

Copyright (c) 2018-2024 Moxi Color

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

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
#ifndef COGO_YIELD_LABELS_AS_VALUES_H_
#define COGO_YIELD_LABELS_AS_VALUES_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CO_STATUS_BEGIN 0
#define CO_STATUS_END   (-1)

// implement yield
typedef struct cogo_yield {
  // Start point (address) where coroutine function continue to run after yield.
  //  0: inited
  // -1: finished
  intptr_t pc;
} cogo_yield_t;

// cogo_yield_t.pc
#define COGO_PC(CO) (((cogo_yield_t*)(CO))->pc)

#define COGO_BEGIN(CO)                                                                            \
  switch (COGO_PC(CO)) {                                                                          \
    case CO_STATUS_BEGIN:                                                                         \
      goto cogo_begin;                                                                            \
      /* eliminate warning of unused label */                                                     \
      goto cogo_return;                                                                           \
      /* eliminate clang error: indirect goto in function with no address-of-label expressions */ \
      COGO_PC(CO) = (intptr_t)(&&cogo_begin);                                                     \
    case CO_STATUS_END:                                                                           \
      goto cogo_end;                                                                              \
    default:                                                                                      \
      goto*(void*)COGO_PC(CO);                                                                    \
  }                                                                                               \
  cogo_begin

#define COGO_YIELD(CO)                                                  \
  do {                                                                  \
    COGO_PC(CO) = (intptr_t)(&&COGO_LABEL); /* 1. save restore point */ \
    goto cogo_end;                          /* 2. return */             \
  COGO_LABEL:;                              /* 3. restore point */      \
  } while (0)

#define COGO_RETURN(CO) \
  goto cogo_return /* end coroutine */

#define COGO_END(CO)                \
  cogo_return:;                     \
  /**/ COGO_PC(CO) = CO_STATUS_END; \
  cogo_end

// Make goto label.
// e.g. COGO_LABEL -> COGO_LABEL(__LINE__) -> cogo_yield_118
#define COGO_LABEL       COGO_LABEL1(__LINE__)
#define COGO_LABEL1(...) COGO_LABEL2(__VA_ARGS__)
#define COGO_LABEL2(N)   cogo_yield_##N

typedef intptr_t co_status_t;
#define CO_STATUS(CO) ((co_status_t)COGO_PC(CO))

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_LABELS_AS_VALUES_H_
