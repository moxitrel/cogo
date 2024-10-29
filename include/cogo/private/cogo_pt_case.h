// Copyright (c) 2018-2024 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

/// @file
/// @warning Undefined behavior if CO_YIELD used in the **case** statement.
#ifndef COGO_PT_CASE_H_
#define COGO_PT_CASE_H_

/// Invalid pc handler. Invoked when pc isn't valid.
/// You must redefine this macro (undef it first, then define it again) or define it before including the header if you want to customize.
#ifndef COGO_ON_EPC
#define COGO_ON_EPC(PT)  // noop
#endif

/// Invoked when coroutine begin to run for the first time.
#ifndef COGO_ON_BEGIN
#define COGO_ON_BEGIN(PT)  // noop
#endif

/// Invoked when COGO_YIELD is called.
#ifndef COGO_ON_YIELD
#define COGO_ON_YIELD(PT)  // noop
#endif

/// Invoked when coroutine resumed (continue to run).
#ifndef COGO_ON_RESUME
#define COGO_ON_RESUME(PT)  // noop
#endif

/// Invoked when COGO_RETURN is called.
#ifndef COGO_ON_RETURN
#define COGO_ON_RETURN(PT)  // noop
#endif

/// Invoked when coroutine end (finished).
#ifndef COGO_ON_END
#define COGO_ON_END(PT)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// The position where function has reached.
typedef int cogo_pc_t;

/// An opaque (all fields are private, and shouldn't be accessed by user directly) object type represents a coroutine.
typedef struct cogo_pt {
  // Source line (`__LINE__`) where function continues to run when reentered.
  // It must be initialized to `0`.
  // It must be set to `-1` if it has finished running.
  cogo_pc_t private_pc;
} cogo_pt_t;

/// The coroutine is initialized, and ready to run.
#define COGO_PC_BEGIN 0
/// The coroutine has finished running.
#define COGO_PC_END   (-1)
/// Get pc as an rvalue to prevent it from being tampered with by assignment (e.g. `COGO_PC(PT) = 0`).
#define COGO_PC(PT)   ((PT)->private_pc)

/// Coroutine begin label.
/// - There must be a `COGO_END` after `COGO_BEGIN`.
/// - There should be only one `COGO_BEGIN` and `COGO_END` in a function.
/// - `COGO_ON_BEGIN` is called if it's defined and the coroutine runs the first time.
///
/// @param PT The coroutine object pointer that has the type of `cogo_pt_t*`.
/// - It must not be `nullptr`.
/// - The expression of `PT` must have no side effects (e.g. e++, e -= v), or the behavior is undefined.
///
/// @exception
/// - `COGO_ON_EPC` is called if it's defined and the resume point is an invalid value.
///
/// @par Example
/// @code
/// void func(cogo_pt_t* pt) {
/// COGO_BEGIN(pt):
///
///   // User codes.
///
/// COGO_END(pt):;
/// }
/// @endcode
#define COGO_BEGIN(PT)                                                                      \
  switch (COGO_PC(PT)) {                                                                    \
    default:                 /* invalid pc */                                               \
      COGO_ON_EPC((&*(PT))); /* Convert `PT` to an rvalue to prevent tampering. */          \
      goto cogo_end;                                                                        \
      goto cogo_return; /* redundant statement: to eliminate the warning of unused label */ \
      goto cogo_begin;  /* redundant statement: to eliminate the warning of unused label */ \
    case COGO_PC_END:                                                                       \
      goto cogo_end;                                                                        \
    case COGO_PC_BEGIN:                                                                     \
      COGO_ON_BEGIN((&*(PT)));                                                              \
      cogo_begin /* coroutine begin label */

/// Jump to `COGO_END`, and the next run will start from here.
/// - Undefined behavior if `COGO_YIELD` used in the **case** statements.
/// - `COGO_ON_YIELD` is called if it's defined.
/// - `COGO_ON_RESUME` is called if it's defined and the coroutine is reentered.
///
/// @param PT The coroutine object pointer that has the type of `cogo_pt_t*`.
/// - It must not be `nullptr`.
/// - The expression of `PT` must have no side effects (e.g. e++, e -= v), or the behavior is undefined.
/// - The object referenced by `PT` must be the same one as passed to `COGO_BEGIN` and `COGO_END`.
///
/// @par Example
/// @code
/// void natural_number_generator(cogo_pt_t* pt, int* v) {
/// COGO_BEGIN(pt):
///
///   for (int i = 0; ; i++) {
///     *v = i;
///     COGO_YIELD(pt); // Pause the execution and return. The next run will start from here.
///   }
///
/// COGO_END(pt):;
/// }
/// @endcode
#define COGO_YIELD(PT)                                                      \
  do {                                                                      \
    COGO_ON_YIELD((&*(PT)));                                                \
    COGO_PC(PT) = __LINE__; /* 1. save the resume point (case __LINE__:) */ \
    goto cogo_end;          /* 2. return */                                 \
    case __LINE__:          /* 3. resume point */                           \
      COGO_ON_RESUME((&*(PT)));                                             \
  } while (0)

/// Jump to COGO_END, and end the coroutine.
/// When a coroutine is ended, the coroutine body (between COGO_BEGIN and COGO_END) will be skipped if run again.
/// @param PT The value of PT should point to an object which inherit from cogo_pt_t.
/// And the object referenced by PT must be the same one as passed to COGO_BEGIN and COGO_END.
/// It must not be nullptr.
/// The expression of PT must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_RETURN(PT)                   \
  do {                                    \
    COGO_ON_RETURN((&*(PT)));             \
    goto cogo_return; /* end coroutine */ \
  } while (0)

/// Coroutine end label.
/// There must be a corresponding @ref COGO_BEGIN before in the same function.
/// And there should be only one COGO_BEGIN and COGO_END in a function.
/// @param PT The value of PT should point to an object which inherit from cogo_pt_t.
/// And the object referenced by PT must be the same one as passed to COGO_BEGIN.
/// It must not be nullptr.
/// The expression of PT must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_END(PT)         \
  cogo_return:               \
  COGO_ON_END((&*(PT)));     \
  COGO_PC(PT) = COGO_PC_END; \
  }                          \
  cogo_end

#define COGO_T    cogo_pt_t
#define COGO_PT   cogo_this
#define CO_BEGIN  COGO_BEGIN(COGO_PT)
#define CO_END    COGO_END(COGO_PT)
#define CO_YIELD  COGO_YIELD(COGO_PT)
#define CO_RETURN COGO_RETURN(COGO_PT)

#ifdef __cplusplus
}
#endif
#endif  // COGO_PT_CASE_H_
