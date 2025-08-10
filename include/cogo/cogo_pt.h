// Copyright (c) 2018-2025 Moxi Color
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
#ifndef COGO_PT_H_
#define COGO_PT_H_

/// @hideinitializer The coroutine start handler, which is invoked when the function reaches the `COGO_BEGIN` label for the first time during its execution.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine it after inclusion.
#ifndef COGO_ON_BEGIN
    #define COGO_ON_BEGIN(COGO)  // noop
#endif

/// @hideinitializer The coroutine finish handler, which is invoked when the function reaches the `COGO_END` label.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (`#undef COGO_ON_END` first, and then `#define` again) it after inclusion.
#ifndef COGO_ON_END
    #define COGO_ON_END(COGO)  // noop
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

/// @hideinitializer The invalid pc handler, which is invoked when the coroutine's pc value isn't valid.
/// The default behavior results in no action.
/// @param[in] COGO The pointer to the current coroutine context object, which holds the states for the ongoing coroutine execution.
/// @note You should define this macro before including the header file, or redefine (`#undef COGO_ON_END` first, and then `#define` again) it after inclusion.
#ifndef COGO_ON_EPC
    #define COGO_ON_EPC(COGO)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(COGO_DEBUG) && defined(assert)
    #define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
    #define COGO_ASSERT(...)  // noop
#endif

#define COGO_NOOP(...)  // noop

#ifndef COGO_PC_T
    #define COGO_PC_T int
#endif
#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)

typedef struct cogo_pt {
    // The source line where function continues to run when reentered.
    // It should be initialized to `0`, and set to `-1` if the coroutine finished running.
    COGO_PC_T pc;
} cogo_pt_t;

#define COGO_PT_T            cogo_pt_t
#define COGO_PT_INIT()       {/*.pc=*/0}
#define COGO_PT_PC(PT)       ((PT)->pc)
#define COGO_PT_IS_VALID(PT) (COGO_PT_PC(PT) >= COGO_PC_END)
#define COGO_PT_BEGIN(PT)    COGO_DO_BEGIN(PT, PT)
#define COGO_PT_YIELD(PT)    COGO_DO_YIELD(PT, COGO_PT_PC(PT), COGO_ON_YIELD, COGO_ON_RESUME)
#define COGO_PT_RETURN(PT)   COGO_DO_RETURN(PT, PT)
#define COGO_PT_END(PT)      COGO_DO_END(PT, PT)
#define COGO_PT_STATUS(PT)   (+COGO_PT_PC(PT))
#define CO_PT_BEGIN          COGO_PT_BEGIN(COGO_THIS)
#define CO_PT_YIELD          COGO_PT_YIELD(COGO_THIS)
#define CO_PT_RETURN         COGO_PT_RETURN(COGO_THIS)
#define CO_PT_END            COGO_PT_END(COGO_THIS)

/// Coroutine type
#ifndef COGO_T
    #define COGO_T COGO_PT_T
#endif

/// @hideinitializer Initialize a coroutine object.
/// @param[in] COGO The coroutine object pointer.
/// @param[in] FUNC The coroutine function pointer.
/// @pre `(COGO) != nullptr`
/// @pre `(FUNC) != nullptr`
#define COGO_INIT(COGO, FUNC) COGO_PT_INIT()
#define COGO_PT_OF(COGO)      (COGO)
#define COGO_PC_OF(COGO)      COGO_PT_PC(COGO_PT_OF(COGO))
#define COGO_IS_VALID(COGO)   ((COGO) == (COGO) && (COGO) && COGO_PT_IS_VALID(COGO))

/// @hideinitializer A label-like macro marks the start of the coroutine.
/// - If the coroutine runs for the first time, `COGO_ON_BEGIN(COGO)` is invoked, and then continues its execution.
/// - If the coroutine is reentered after a yield, the execution will jump to the next line of last `COGO_YIELD(COGO)`.
/// - If the coroutine has finished running, the execution will jump to the `COGO_END(COGO)` label.
/// - If the resume point is invalid, `COGO_ON_EPC(COGO)` is invoked, and then jump to the `COGO_END(COGO)` label.
/// @param[in] COGO The coroutine object pointer.
/// @pre `(COGO) != nullptr && !__has_side_effects(COGO)`
/// @pre There should be one and only one `COGO_END(COGO)` after the `COGO_BEGIN(COGO)` in a function.
#define COGO_BEGIN(COGO)      COGO_DO_BEGIN(COGO, COGO_PT_OF(COGO))

/// @hideinitializer Jump to `COGO_END`, and the next run will start from here.
/// - Undefined behavior if `COGO_YIELD` used in the **case** statements.
/// @param[in] COGO The coroutine object pointer that has the type of `COGO_T*`.
/// @pre `(COGO) != nullptr && !__has_side_effects(COGO)`
/// @pre `COGO` must be the same one as passed to `COGO_BEGIN` and `COGO_END`.
/// @post `COGO_ON_YIELD` is called if it's defined before yield.
/// @post `COGO_ON_RESUME` is called if it's defined and the coroutine is reentered.
#define COGO_YIELD(COGO)      COGO_DO_YIELD(COGO, COGO_PT_OF(COGO), COGO_ON_YIELD, COGO_ON_RESUME)

/// @hideinitializer Jump to COGO_END, and finish the coroutine.
/// When a coroutine is finished, the coroutine body (between COGO_BEGIN and COGO_END) will be skipped if run again.
/// @param[in] COGO The value of `COGO` should point to an object which inherit from `COGO_T`.
/// And the object referenced by `COGO` must be the same one as passed to `COGO_BEGIN` and `COGO_END`.
/// @pre `(COGO) != nullptr`
#define COGO_RETURN(COGO)     COGO_DO_RETURN(COGO, COGO_PT_OF(COGO))

/// @hideinitializer A label-like macro marks the end of the coroutine.
/// There must be a corresponding `COGO_BEGIN` before in the same function.
/// And there should be only one `COGO_BEGIN` and `COGO_END` in a function.
/// @param[in] COGO The value of `COGO` should point to an object which inherit from `COGO_T`.
/// @pre `(COGO) != nullptr && !__has_side_effects(COGO)`
/// @pre The object referenced by `COGO` must be the same one that was passed to `COGO_BEGIN`.
#define COGO_END(COGO)        COGO_DO_END(COGO, COGO_PT_OF(COGO))

/// @hideinitializer The return type of `COGO_STATUS(COGO)`.
#define COGO_STATUS_T         COGO_PC_T

/// @hideinitializer An integer value indicates the coroutine is initialized and ready to run.
#define COGO_STATUS_BEGIN     COGO_PC_BEGIN

/// @hideinitializer An integer value indicates the coroutine has finished running.
#define COGO_STATUS_END       COGO_PC_END

/// @hideinitializer Get the current running status of coroutine.
/// @param[in] COGO Point to a coroutine object.
/// @return An integer value indicates the coroutine is in running.
/// @retval `COGO_STATUS_BEGIN` The coroutine is initialized and not invoked yet.
/// @retval `COGO_STATUS_END` The coroutine has finished running.
/// @pre `(COGO) != nullptr`.
#define COGO_STATUS(COGO)     COGO_PT_STATUS(COGO_PT_OF(COGO))

/// Abbr of `COGO_BEGIN(COGO_THIS)`.
#define CO_BEGIN              COGO_BEGIN(COGO_THIS)
/// Abbr of `COGO_YIELD(COGO_THIS)`.
#define CO_YIELD              COGO_YIELD(COGO_THIS)
/// Abbr of `COGO_RETURN(COGO_THIS)`.
#define CO_RETURN             COGO_RETURN(COGO_THIS)
/// Abbr of `COGO_END(COGO_THIS)`.
#define CO_END                COGO_END(COGO_THIS)

#define COGO_DO_BEGIN(COGO, PT)                                                                    \
    switch (COGO_PT_PC(PT)) {                                                                      \
        default:                    /* `PC` isn't valid. */                                        \
            COGO_ON_EPC((+(COGO))); /* Convert `COGO` to an rvalue to prevent tampering. */        \
            goto cogo_end;                                                                         \
            goto cogo_return; /* Redundant statement: to eliminate the warning of unused label. */ \
            goto cogo_begin;  /* Redundant statement: to eliminate the warning of unused label. */ \
        case COGO_PC_END:                                                                          \
            goto cogo_end;                                                                         \
        case COGO_PC_BEGIN:                                                                        \
            COGO_ON_BEGIN((+(COGO)));                                                              \
            cogo_begin /* The coroutine begin label. */

#define COGO_DO_YIELD(COGO, PT, ON_YIELD, ON_RESUME)                                       \
    do {                                                                                   \
        ON_YIELD((+(COGO)));                                                               \
        COGO_PT_PC(PT) = __LINE__; /* 1. save the resume point (i.e., `case __LINE__:`) */ \
        goto cogo_end;             /* 2. return */                                         \
        case __LINE__:;            /* 3. resume point */                                   \
            ON_RESUME((+(COGO)));                                                          \
    } while (0)

#define COGO_DO_RETURN(COGO, PT)   \
    do {                           \
        COGO_ON_RETURN((+(COGO))); \
        goto cogo_return;          \
    } while (0)

#define COGO_DO_END(COGO, PT)     \
cogo_return:                      \
    COGO_ON_END((+(COGO)));       \
    COGO_PT_PC(PT) = COGO_PC_END; \
    } /* End of switch */         \
    cogo_end /* The coroutine end label. */

#ifdef __cplusplus
}
#endif
#endif  // COGO_PT_H_
