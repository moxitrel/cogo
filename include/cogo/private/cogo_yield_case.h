/* Copyright (c) 2018-2024 Moxi Color

Use of this source code is governed by a MIT-style license
that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

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
    switch (co_this->line) {    // CO_BEGIN:
    case  0:                    //

        for (co_this->i = 0; ;co_this->i++) {

            co_this->line = 11; //
            return;             // CO_YIELD;
    case 11:;                   //

        }

    }                           // CO_END:
}

* Drawbacks
- Undefined behavior if COGO_YIELD() used in *case* statement.

* See Also
- Coroutines in C   (https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html)
- Protothreads      (http://dunkels.com/adam/pt/expansion.html)

*/
#ifndef COGO_YIELD_CASE_H_
#define COGO_YIELD_CASE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef int cogo_pc_t;

// implement yield
typedef struct cogo_yield {
  // Source line (__LINE__) where function continues to run after yield.
  //   0: begin (run first time)
  //  -1: end (finished)
  cogo_pc_t private_pc;
} cogo_yield_t;

#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)
// cogo_yield_t.private_pc (lvalue)
#define COGO_PC(CO)   (((cogo_yield_t*)(CO))->private_pc)

// CO should has no side effects if COGO_ON_EPC(),  COGO_ON_END() or COGO_ON_BEGIN() is defined.
#define COGO_BEGIN(CO)                                                                      \
  switch (COGO_PC(CO)) {                                                                    \
    default: /* invalid pc */                                                               \
      /* pass as rvalue to prevent value from tampered by user */                           \
      COGO_ON_EPC(((void const*)(CO)), ((cogo_pc_t)COGO_PC(CO)));                           \
      goto cogo_end;                                                                        \
      goto cogo_return; /* redundant statement: to eliminate the warning of unused label */ \
      goto cogo_begin;  /* redundant statement: to eliminate the warning of unused label */ \
    case COGO_PC_END:   /* coroutine end */                                                 \
      goto cogo_end;                                                                        \
    case COGO_PC_BEGIN: /* coroutine begin */                                               \
      COGO_ON_BEGIN(((void const*)(CO)));                                                   \
      cogo_begin /* coroutine begin label */

// CO should has no side effects if COGO_ON_YIELD() is defined.
#define COGO_YIELD(CO)                                                       \
  do {                                                                       \
    COGO_ON_YIELD(((void const*)(CO)), __LINE__);                            \
    COGO_PC(CO) = __LINE__; /* 1. save the restore point (case __LINE__:) */ \
    goto cogo_end;          /* 2. return */                                  \
    case __LINE__:          /* 3. restore point */                           \
      COGO_ON_RESUME(((void const*)(CO)), __LINE__);                         \
  } while (0)

#define COGO_RETURN(CO)                   \
  do {                                    \
    COGO_ON_RETURN(((void const*)(CO)));  \
    goto cogo_return; /* end coroutine */ \
  } while (0)

// CO should has no side effects if COGO_ON_END() is defined.
#define COGO_END(CO)                \
  cogo_return:                      \
  COGO_ON_END(((void const*)(CO))); \
  COGO_PC(CO) = COGO_PC_END;        \
  }                                 \
  cogo_end

// Invoked when pc isn't valid.
#ifndef COGO_ON_EPC
#define COGO_ON_EPC(CO, PC)  // noop
#endif

// Invoked when coroutine begin (enter for the first time).
#ifndef COGO_ON_BEGIN
#define COGO_ON_BEGIN(CO)  // noop
#endif

// Invoked when COGO_YIELD() is called.
#ifndef COGO_ON_YIELD
#define COGO_ON_YIELD(CO, NEXT_PC)  // noop
#endif

// Invoked when coroutine resumed (continue to run).
#ifndef COGO_ON_RESUME
#define COGO_ON_RESUME(CO, NEXT_PC)  // noop
#endif

// Invoked when COGO_RETURN() is called.
#ifndef COGO_ON_RETURN
#define COGO_ON_RETURN(CO)  // noop
#endif

// Invoked when coroutine end (finished).
#ifndef COGO_ON_END
#define COGO_ON_END(CO)  // noop
#endif

typedef cogo_pc_t co_status_t;
#define CO_STATUS_BEGIN COGO_PC_BEGIN
#define CO_STATUS_END   COGO_PC_END
#define CO_STATUS(CO)   ((co_status_t)COGO_PC(CO))  // get as rvalue

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_CASE_H_
