// Copyright (c) 2018-2024 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

/// @file
///
/// @par Example
/// @code
/// void number_generator(COGO_T* cogo, int* v) {
/// COGO_BEGIN(cogo): // Coroutine begin.
///
///   for (;; (*v)++) {
///     COGO_YIELD(cogo); // Return. The next run will start from here.
///   }
///
/// COGO_END(cogo): // Coroutine end.
///   ;
/// }
/// @endcode
///
/// @warning Undefined behavior if COGO_YIELD used in the **case** statement.
#ifndef COGO_PT_CASE_H_
#define COGO_PT_CASE_H_

#ifdef __cplusplus
extern "C" {
#endif

/// @hideinitializer The invalid pc handler, which is invoked when the coroutine's pc value isn't valid.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (`#undef COGO_ON_END` first, and then `#define` again) it after inclusion.
#ifndef COGO_ON_EPC
  #define COGO_ON_EPC(COGO)  // noop
#endif

/// @hideinitializer The coroutine start handler, which is invoked when the coroutine function reaches the `COGO_BEGIN` label for the first time during its execution.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (undef first, then define again) it after inclusion.
#ifndef COGO_ON_BEGIN
  #define COGO_ON_BEGIN(COGO)  // noop
#endif

/// @hideinitializer The coroutine yield handler, which is invoked when `COGO_YIELD` is called.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (undef first, then define again) it after inclusion.
#ifndef COGO_ON_YIELD
  #define COGO_ON_YIELD(COGO)  // noop
#endif

/// @hideinitializer Invoked when coroutine resumed (continue to run).
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (undef first, then define again) it after inclusion.
#ifndef COGO_ON_RESUME
  #define COGO_ON_RESUME(COGO)  // noop
#endif

/// @hideinitializer The coroutine return handler, which is invoked when `COGO_RETURN` is called.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (undef first, then define again) it after inclusion.
#ifndef COGO_ON_RETURN
  #define COGO_ON_RETURN(COGO)  // noop
#endif

/// @hideinitializer The coroutine finish handler, which is invoked when the function reaches the `COGO_END` label.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (`#undef COGO_ON_END` first, and then `#define` again) it after inclusion.
#ifndef COGO_ON_END
  #define COGO_ON_END(COGO)  // noop
#endif

#if defined(COGO_DEBUG) && defined(assert)
  #define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
  #define COGO_ASSERT(...)  // noop
#endif

/// The position where function has reached.
typedef int cogo_pc_t;
/// @hideinitializer A `cogo_pc_t` value indicates the coroutine has finished running.
#define COGO_PC_END   (-1)
/// The zero value of type `cogo_pc_t`, that indicates the coroutine is initialized and ready to run.
#define COGO_PC_BEGIN 0

// The coroutine context type implement yield.
// All fields are protected, and shouldn't be accessed by user directly.
typedef struct cogo_pt {
  // The source line where function continues to run when reentered.
  // It is initialized to `COGO_PC_BEGIN`, set to `__LINE__` when yield, or set to `COGO_PC_END` if finished running.
  cogo_pc_t pc;
} cogo_pt_t;

// Get the cogo_pt_t object pointer from derivant.
#define COGO_PT_OF(PT) (PT)

/// @hideinitializer Get pc as rvalue to prevent it from being tampered with by assignment. e.g., `COGO_PC(COGO) = 0`.
/// @pre `COGO != NULL`.
#define COGO_PC(COGO)  (+COGO_PT_OF(COGO)->pc)

/// @hideinitializer A label-like macro marks the start of the coroutine.
/// @param[in] COGO The coroutine object pointer.
/// @pre `COGO != NULL`.
/// @pre The expanded expression of `COGO` should have no side effects, e.g., `e++`; otherwise, its behavior is undefined.
/// @pre There must be a `COGO_END(COGO)` after `COGO_BEGIN(COGO)`.
/// @pre There should be only one `COGO_BEGIN()` and `COGO_END()` in a function.
/// @post If the coroutine runs for the first time, `COGO_ON_BEGIN(COGO)` is invoked first, and then the coroutine continues its execution.
/// @post If the coroutine is reentered after a yield, the execution will jump to the last `COGO_YIELD()`.
/// @post If the coroutine has finished running, the execution will jump to the `COGO_END()` label.
/// @post If the resume point is invalid, `COGO_ON_EPC(COGO)` is invoked first, and then jump to `COGO_END()`.
#define COGO_BEGIN(COGO)                                                                          \
  COGO_ASSERT((COGO) == (COGO) && (COGO)); /* `COGO` must have no side effects and not `NULL`. */ \
  switch (COGO_PC(COGO)) {                                                                        \
    default:                  /* Invalid pc */                                                    \
      COGO_ON_EPC((+(COGO))); /* Convert `COGO` to an rvalue to prevent tampering. */             \
      goto cogo_end;                                                                              \
      goto cogo_return; /* Redundant statement: to eliminate the warning of unused label. */      \
      goto cogo_begin;  /* Redundant statement: to eliminate the warning of unused label. */      \
    case COGO_PC_END:                                                                             \
      goto cogo_end;                                                                              \
    case COGO_PC_BEGIN:                                                                           \
      COGO_ON_BEGIN((+(COGO)));                                                                   \
      cogo_begin /* coroutine begin label */

/// @hideinitializer Jump to `COGO_END`, and the next run will start from here.
/// - Undefined behavior if `COGO_YIELD` used in the **case** statements.
/// @param[in] COGO The coroutine object pointer that has the type of `cogo_pt_t*`.
/// @pre `COGO != NULL`
/// @pre `COGO` should has no side effects, or the behavior is undefined (e.g. e++, e -= v).
/// @pre `COGO` must be the same one as passed to `COGO_BEGIN` and `COGO_END`.
/// @post `COGO_ON_YIELD` is called if it's defined before yield.
/// @post `COGO_ON_RESUME` is called if it's defined and the coroutine is reentered.
#define COGO_YIELD(COGO)                     \
  do {                                       \
    COGO_ASSERT((COGO) == (COGO) && (COGO)); \
    COGO_ON_YIELD((+(COGO)));                \
    COGO_DO_YIELD(COGO);                     \
    COGO_ON_RESUME((+(COGO)));               \
  } while (0)

#define COGO_DO_YIELD(COGO)                                                          \
  do {                                                                               \
    COGO_PT_OF(COGO)->pc = __LINE__; /* 1. save the resume point (case __LINE__:) */ \
    goto cogo_end;                   /* 2. return */                                 \
    case __LINE__:;                  /* 3. resume point */                           \
  } while (0)

/// @hideinitializer Jump to COGO_END, and finish the coroutine.
/// When a coroutine is finished, the coroutine body (between COGO_BEGIN and COGO_END) will be skipped if run again.
/// @param[in] COGO The value of COGO should point to an object which inherit from cogo_pt_t.
/// And the object referenced by COGO must be the same one as passed to COGO_BEGIN and COGO_END.
/// It must not be NULL.
/// The expression of COGO must have no side effects (e.g. e++, e -= v) which may cause undefined behavior.
#define COGO_RETURN(COGO)                    \
  do {                                       \
    COGO_ASSERT((COGO) == (COGO) && (COGO)); \
    COGO_ON_RETURN((+(COGO)));               \
    goto cogo_return; /* end coroutine */    \
  } while (0)

/// @hideinitializer A label-like macro marks the end of the coroutine.
/// There must be a corresponding @ref COGO_BEGIN before in the same function.
/// And there should be only one COGO_BEGIN and COGO_END in a function.
/// @param[in] COGO The value of COGO should point to an object which inherit from cogo_pt_t.
/// @pre `COGO != NULL`.
/// @pre The expanded expression of `COGO` should have no side effects, e.g., `e++`; otherwise, its behavior is undefined.
/// @pre The object referenced by `COGO` must be the same one that was passed to `COGO_BEGIN`.
#define COGO_END(COGO)                     \
cogo_return:                               \
  COGO_ASSERT((COGO) == (COGO) && (COGO)); \
  COGO_ON_END((+(COGO)));                  \
  COGO_PT_OF(COGO)->pc = COGO_PC_END;      \
  } /* switch */                           \
  cogo_end

/// @var COGO_T* COGO_THIS
/// The implicit variable representing the current coroutine object, which is used by CO_* macros (e.g., CO_BEGIN, CO_YIELD, ...).

#define CO_BEGIN  COGO_BEGIN(COGO_THIS)
#define CO_END    COGO_END(COGO_THIS)
#define CO_YIELD  COGO_YIELD(COGO_THIS)
#define CO_RETURN COGO_RETURN(COGO_THIS)

/// @hideinitializer An opaque object type that saves the coroutine states.
#ifndef COGO_T
  #define COGO_T cogo_pt_t
#endif

#ifdef __cplusplus
}
#endif
#endif  // COGO_PT_CASE_H_
