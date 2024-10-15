/* MIT License

Copyright (c) 2024 Moxi Color

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

* Use Duff's Device (Protothreads)

* API

COGO_BEGIN()    : coroutine begin.
COGO_END()      : coroutine end.
COGO_YIELD()    : yield from coroutine.
COGO_RETURN()   : return from coroutine.

co_status_t       : type of CO_STATUS().
CO_STATUS()       : get the current running status.
  CO_STATUS_BEGIN : inited
  CO_STATUS_END   : finished

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
    switch (co_this->pc) {          // CO_BEGIN:
    case  0:                        //

        for (co_this->i = 0; ;co_this->i++) {

            co_this->pc = 11;       //
            return;                 // CO_YIELD;
    case 11:;                       //

        }

    }                               // CO_END:
}

* Drawbacks
- No CO_YIELD allowed in *case* statement.

* See Also
- Coroutines in C   (https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html)
- Protothreads      (http://dunkels.com/adam/pt/expansion.html)

*/
#ifndef COGO_YIELD_SWITCH_H_
#define COGO_YIELD_SWITCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Invoked when COGO_YIELD() is called. Shouldn't modify coroutine attributes, e.g. pc.
#ifndef COGO_ON_YIELD
#define COGO_ON_YIELD(CO, PC) /*noop*/
#endif

// Invoked when COGO_RETURN() is called. Shouldn't modify coroutine attributes, e.g. pc.
#ifndef COGO_ON_RETURN
#define COGO_ON_RETURN(CO) /*noop*/
#endif

// Invoked when coroutine finished. Shouldn't modify coroutine attributes, e.g. pc.
#ifndef COGO_ON_END
#define COGO_ON_END(CO) /*noop*/
#endif

// Invoked when pc isn't valid. Shouldn't modify coroutine attributes, e.g. pc.
#ifndef COGO_ON_EPC
#define COGO_ON_EPC(CO, PC) /*noop*/
#endif

// implement yield
typedef struct cogo_yield {
  // Start point (__LINE__) where function continues to run after yield.
  //  >0: running
  //   0: inited
  //  -1: finished
  int pc;
} cogo_yield_t;

#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)
// cogo_yield_t.pc (lvalue)
#define COGO_PC(CO)   (((cogo_yield_t*)(CO))->pc)

// CO should has no side effects if COGO_ON_EPC(),  COGO_ON_END() or COGO_ON_BEGIN() is defined.
#define COGO_BEGIN(CO)                                            \
  switch (COGO_PC(CO)) {                                          \
    default: /* invalid pc */                                     \
      COGO_ON_EPC(((void const* const)(CO)), ((int)COGO_PC(CO))); \
      goto cogo_end;                                              \
      goto cogo_return; /* eliminate warning of unused label */   \
    case COGO_PC_END:   /* coroutine end */                       \
      COGO_ON_END(((void const* const)(CO)));                     \
      goto cogo_end;                                              \
    case COGO_PC_BEGIN /* coroutine begin */

// CO should has no side effects if COGO_ON_YIELD() is defined.
#define COGO_YIELD(CO)                                                            \
  do {                                                                            \
    /**/ COGO_ON_YIELD(((void const* const)(CO)), __LINE__);                      \
    /**/ COGO_PC(CO) = __LINE__; /* 1. save the restore point (case __LINE__:) */ \
    /**/ goto cogo_end;          /* 2. return */                                  \
    case __LINE__:;              /* 3. restore point */                           \
  } while (0)

#define COGO_RETURN(CO)                             \
  do {                                              \
    /**/ COGO_ON_RETURN(((void const* const)(CO))); \
    /**/ goto cogo_return; /* end coroutine */      \
  } while (0)

#define COGO_END(CO)              \
  cogo_return:                    \
  /**/ COGO_PC(CO) = COGO_PC_END; \
  }                               \
  cogo_end

typedef int co_status_t;
#define CO_STATUS_BEGIN COGO_PC_BEGIN
#define CO_STATUS_END   COGO_PC_END
#define CO_STATUS(CO)   ((co_status_t)COGO_PC(CO))  // current running status (as rvalue)

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_SWITCH_H_
