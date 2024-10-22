// Copyright (c) 2018-2024 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

/** @file
* Use Duff's Device (Protothreads)

* Example
void nat_func(nat_t* cogo_this)
{
CO_BEGIN:

    for (cogo_this->i = 0; ;cogo_this->i++) {
        CO_YIELD;
    }

CO_END:;
}

* Internal
void nat_func(nat_t* cogo_this)
{
    switch (cogo_this->line) {    // CO_BEGIN:
    case  0:                    //

        for (cogo_this->i = 0; ;cogo_this->i++) {

            cogo_this->line = 11; //
            return;             // CO_YIELD;
    case 11:;                   //

        }

    }                           // CO_END:
}

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

/// An opaque (all fields are private and shouldn't be accessed by user directly) object type represent a coroutine.
typedef struct cogo_yield {
  // Source line (`__LINE__`) where function continues to run when reentered.
  // It must be initialized to `0`.
  // It must be set to `-1` if it has finished running.
  cogo_pc_t private_pc;
} cogo_yield_t;

/// The coroutine is initialized, and ready to run.
#define COGO_PC_BEGIN 0
/// The coroutine has finished running.
#define COGO_PC_END   (-1)
/// Get running status as rvalue to prevent pc from changing by assignment (`COGO_PC(CO) = v`).
#define COGO_PC(CO)   ((cogo_pc_t)((cogo_yield_t*)(CO))->private_pc)

/// Coroutine begin label.
/// - There must be a `COGO_END` after `COGO_BEGIN`.
/// - There should be only one `COGO_BEGIN` and `COGO_END` in a function.
/// - `COGO_ON_BEGIN` is called if it's defined and the coroutine runs the first time.
///
/// @param CO Coroutine object.
/// - The value of `CO` should point to an object which inherit from `cogo_yield_t`.
/// - It must not be `nullptr`.
/// - The expression of `CO` must have no side effects (e.g. e++, e -= v), or the behavior is undefined.
///
/// @exception
/// - `COGO_ON_EPC` is called if it's defined and the resume point is an invalid value.
///
/// @par Example
/// @code
/// void func(cogo_yield_t* co) {
/// COGO_BEGIN(co):
///
///   // User codes.
///
/// COGO_END(co):;
/// }
/// @endcode
#define COGO_BEGIN(/*cogo_yield_t* const*/ CO)                                              \
  switch (COGO_PC(CO)) {                                                                    \
    default: /* invalid pc */                                                               \
      /* Pass as rvalue to prevent from tampered by user. */                                \
      COGO_ON_EPC(((void const*)(CO)), COGO_PC(CO));                                        \
      goto cogo_end;                                                                        \
      goto cogo_return; /* redundant statement: to eliminate the warning of unused label */ \
      goto cogo_begin;  /* redundant statement: to eliminate the warning of unused label */ \
    case COGO_PC_END:   /* coroutine end */                                                 \
      goto cogo_end;                                                                        \
    case COGO_PC_BEGIN: /* coroutine begin */                                               \
      COGO_ON_BEGIN(((void const*)(CO)));                                                   \
      cogo_begin /* coroutine begin label */

/// Jump to COGO_END, and the next run will start from here.
/// Undefined behavior if COGO_YIELD used in **case** statements.
/// @param CO The value of CO should point to an object which inherit from cogo_yield_t.
/// And the object referenced by CO must be the same one as passed to COGO_BEGIN and COGO_END.
/// It must not be nullptr.
/// The expression of CO must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_YIELD(/*cogo_yield_t* const*/ CO)                                                    \
  do {                                                                                            \
    COGO_ON_YIELD(((void const*)(CO)));                                                           \
    ((cogo_yield_t*)(CO))->private_pc = __LINE__; /* 1. save the resume point (case __LINE__:) */ \
    goto cogo_end;                                /* 2. return */                                 \
    case __LINE__:                                /* 3. resume point */                           \
      COGO_ON_RESUME(((void const*)(CO)), __LINE__);                                              \
  } while (0)

/// Jump to COGO_END, and end the coroutine.
/// When a coroutine is ended, the coroutine body (between COGO_BEGIN and COGO_END) will be skipped if run again.
/// @param CO The value of CO should point to an object which inherit from cogo_yield_t.
/// And the object referenced by CO must be the same one as passed to COGO_BEGIN and COGO_END.
/// It must not be nullptr.
/// The expression of CO must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_RETURN(/*cogo_yield_t* const*/ CO) \
  do {                                          \
    COGO_ON_RETURN(((void const*)(CO)));        \
    goto cogo_return; /* end coroutine */       \
  } while (0)

/// Coroutine end label.
/// There must be a corresponding @ref COGO_BEGIN before in the same function.
/// And there should be only one COGO_BEGIN and COGO_END in a function.
/// @param CO The value of CO should point to an object which inherit from cogo_yield_t.
/// And the object referenced by CO must be the same one as passed to COGO_BEGIN.
/// It must not be nullptr.
/// The expression of CO must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_END(/*cogo_yield_t* const*/ CO)       \
  cogo_return:                                     \
  COGO_ON_END(((void const*)(CO)));                \
  ((cogo_yield_t*)(CO))->private_pc = COGO_PC_END; \
  }                                                \
  cogo_end

/// Invalid pc callback. Invoked when pc isn't valid.
#ifndef COGO_ON_EPC
#define COGO_ON_EPC(/*void const**/ CO, /*cogo_pc_t*/ PC)  // noop
#endif

/// Coroutine begin callback. Invoked when coroutine begin (enter for the first time).
#ifndef COGO_ON_BEGIN
#define COGO_ON_BEGIN(/*void const**/ CO)  // noop
#endif

/// Coroutine yield callback. Invoked when COGO_YIELD is called.
#ifndef COGO_ON_YIELD
#define COGO_ON_YIELD(/*void const**/ CO)  // noop
#endif

/// Coroutine resume callback. Invoked when coroutine resumed (continue to run).
#ifndef COGO_ON_RESUME
#define COGO_ON_RESUME(/*void const**/ CO, /*cogo_pc_t*/ PC)  // noop
#endif

/// Coroutine return callback. Invoked when COGO_RETURN is called.
#ifndef COGO_ON_RETURN
#define COGO_ON_RETURN(/*void const**/ CO)  // noop
#endif

/// Coroutine end callback. Invoked when coroutine end (finished).
#ifndef COGO_ON_END
#define COGO_ON_END(/*void const**/ CO)  // noop
#endif

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_CASE_H_
