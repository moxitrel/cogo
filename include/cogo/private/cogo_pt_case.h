// Copyright (c) 2018-2024 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

/// @file
///
/// @par Example
/// @code
/// void natural_number_generator(cogo_pt_t* pt, int* v) {
/// COGO_BEGIN(pt): // Coroutine begin.
///
///   for (*v = 0;; (*v)++) {
///     COGO_YIELD(pt); // Return. The next run will start from here.
///   }
///
/// COGO_END(pt): // Coroutine end.
///   ;
/// }
/// @endcode
///
/// @warning Undefined behavior if CO_YIELD used in the **switch case** statement.
#ifndef COGO_PT_CASE_H_
#define COGO_PT_CASE_H_

/// Invalid pc handler. Invoked when pc isn't valid.
/// You must redefine this macro (undef it first, then define it again) or define it before including the header if you want to customize.
#ifndef COGO_ON_EPC
#define COGO_ON_EPC(COGO)  // noop
#endif

/// Invoked when coroutine begin to run for the first time.
#ifndef COGO_ON_BEGIN
#define COGO_ON_BEGIN(COGO)  // noop
#endif

/// Invoked when COGO_YIELD is called.
#ifndef COGO_ON_YIELD
#define COGO_ON_YIELD(COGO)  // noop
#endif

/// Invoked when coroutine resumed (continue to run).
#ifndef COGO_ON_RESUME
#define COGO_ON_RESUME(COGO)  // noop
#endif

/// Invoked when COGO_RETURN is called.
#ifndef COGO_ON_RETURN
#define COGO_ON_RETURN(COGO)  // noop
#endif

/// Invoked when coroutine end (finished).
#ifndef COGO_ON_END
#define COGO_ON_END(COGO)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// The position where function has reached.
typedef int cogo_pc_t;
/// The coroutine is initialized, and ready to run.
#define COGO_PC_BEGIN 0
/// The coroutine has finished running.
#define COGO_PC_END   (-1)

/// An opaque object type (all fields are protected, and shouldn't be accessed by user directly) represents a coroutine.
typedef struct cogo_pt {
  /// @private The source line (`__LINE__`) where function continues to run when reentered.
  /// It must be initialized to `0`.
  /// It must be set to `-1` if it has finished running.
  cogo_pc_t pc;
} cogo_pt_t;

// Get the cogo_pt_t object from the derived (i.e. COGO_T) object.
#define COGO_PT_V(PT) (PT)

/// Get pc as an rvalue to prevent it from being tampered with by assignment (e.g. `COGO_PC(COGO) = 0`).
#define COGO_PC(COGO) (+COGO_PT_V(COGO)->pc)

/// A label like macro marks the beginning of coroutine.
/// The execution will jump to the last `COGO_YIELD(COGO)` and continue to run.
/// - `COGO_ON_BEGIN(COGO)` is invoked if the coroutine runs the first time.
/// - `COGO_ON_EPC(COGO)` is invoked if the resume point is invalid.
///
/// @param COGO COGO_T*, the coroutine object.
/// - It mustn't be `nullptr`.
/// - Undefined behavior if the expression of `COGO` has side effects (e.g. e++, e -= v).
///
/// @pre
/// - There must be a `COGO_END(COGO)` after `COGO_BEGIN(COGO)`.
/// - There should be only one `COGO_BEGIN` and `COGO_END` in a function.
#define COGO_BEGIN(COGO)                                                                     \
  switch (COGO_PC(COGO)) {                                                                   \
    default:                   /* invalid pc */                                              \
      COGO_ON_EPC((&*(COGO))); /* Convert `COGO` to an rvalue to prevent tampering. */       \
      goto cogo_end;                                                                         \
      goto cogo_return; /* Redundant statement: to eliminate the warning of unused label. */ \
      goto cogo_begin;  /* Redundant statement: to eliminate the warning of unused label. */ \
    case COGO_PC_END:                                                                        \
      goto cogo_end;                                                                         \
    case COGO_PC_BEGIN:                                                                      \
      COGO_ON_BEGIN((&*(COGO)));                                                             \
      cogo_begin /* coroutine begin label */

/// Jump to `COGO_END`, and the next run will start from here.
/// - Undefined behavior if `COGO_YIELD` used in the **case** statements.
/// - `COGO_ON_YIELD` is called if it's defined.
/// - `COGO_ON_RESUME` is called if it's defined and the coroutine is reentered.
///
/// @param COGO The coroutine object pointer that has the type of `cogo_pt_t*`.
/// - It must not be `nullptr`.
/// - The expression of `COGO` must have no side effects (e.g. e++, e -= v), or the behavior is undefined.
/// - The object referenced by `COGO` must be the same one as passed to `COGO_BEGIN` and `COGO_END`.
#define COGO_YIELD(COGO)                                                            \
  do {                                                                              \
    COGO_ON_YIELD((&*(COGO)));                                                      \
    COGO_PT_V(COGO)->pc = __LINE__; /* 1. save the resume point (case __LINE__:) */ \
    goto cogo_end;                  /* 2. return */                                 \
    case __LINE__:                  /* 3. resume point */                           \
      COGO_ON_RESUME((&*(COGO)));                                                   \
  } while (0)

/// Jump to COGO_END, and end the coroutine.
/// When a coroutine is ended, the coroutine body (between COGO_BEGIN and COGO_END) will be skipped if run again.
/// @param COGO The value of COGO should point to an object which inherit from cogo_pt_t.
/// And the object referenced by COGO must be the same one as passed to COGO_BEGIN and COGO_END.
/// It must not be nullptr.
/// The expression of COGO must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_RETURN(COGO)                 \
  do {                                    \
    COGO_ON_RETURN((&*(COGO)));           \
    goto cogo_return; /* end coroutine */ \
  } while (0)

/// A label like macro marks the end of coroutine function.
/// There must be a corresponding @ref COGO_BEGIN before in the same function.
/// And there should be only one COGO_BEGIN and COGO_END in a function.
/// @param COGO The value of COGO should point to an object which inherit from cogo_pt_t.
/// And the object referenced by COGO must be the same one as passed to COGO_BEGIN.
/// It must not be nullptr.
/// The expression of COGO must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_END(COGO)               \
  cogo_return:                       \
  COGO_ON_END((&*(COGO)));           \
  COGO_PT_V(COGO)->pc = COGO_PC_END; \
  }                                  \
  cogo_end

/// An alias type defined to `cogo_pt_t`.
/// The main purpose of this macro is to maintain the consistency with `cogo_yield.h`, `cogo_await.h` and `cogo_async.h`,
/// all of which define `COGO_T`.
#ifndef COGO_T
#define COGO_T cogo_pt_t
#endif

/// @var COGO_T* cogo_this
/// The implicit variable representing the current coroutine object, which is used by CO_* macros (e.g., CO_BEGIN, CO_YIELD, ...).
#define CO_BEGIN  COGO_BEGIN(cogo_this)
#define CO_END    COGO_END(cogo_this)
#define CO_YIELD  COGO_YIELD(cogo_this)
#define CO_RETURN COGO_RETURN(cogo_this)

#ifdef __cplusplus
}
#endif
#endif  // COGO_PT_CASE_H_
