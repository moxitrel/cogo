// Copyright (c) 2018-2025 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

/// @file
#ifndef COGO_CALL_H_
#define COGO_CALL_H_

#ifndef COGO_T
    #define COGO_T COGO_CALL_T
#endif
typedef struct cogo_call cogo_call_t;
#define COGO_CALL_T cogo_call_t

#include "cogo_pt.h"

#ifndef COGO_SCHED_T
    #define COGO_SCHED_T COGO_CALLSCHED_T
#endif
typedef struct cogo_callsched cogo_callsched_t;
#define COGO_CALLSCHED_T cogo_callsched_t

#ifndef COGO_BEFORE_AWAIT
    #define COGO_BEFORE_AWAIT(COGO, COGO_OTHER)  // noop
#endif

#ifndef COGO_AFTER_AWAIT
    #define COGO_AFTER_AWAIT(COGO, COGO_OTHER)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct cogo_call {
    COGO_PT_T pt;

    // The coroutine function.
    void (*func)(COGO_T*);

    // build call stack
    COGO_T* caller;

    union {
        // resume point
        COGO_T* top;

        // associated scheduler
        COGO_SCHED_T* sched;
    } a;
};

const COGO_CALL_T* cogo_call_resume(COGO_CALL_T* call);

struct cogo_callsched {
    // call stack top
    COGO_T* top;
};

COGO_CALL_T* cogo_callsched_resume(COGO_CALLSCHED_T* callsched);

#define COGO_CALL_INIT(COGO, FUNC)   \
    {                                \
            /*.pt=*/COGO_PT_INIT(),  \
            /*.func=*/(FUNC),        \
            /*.caller=*/0,           \
            /*.a=*/{                 \
                    /*.top=*/(COGO), \
            },                       \
    }
#define COGO_CALL_PT(CALL)                (&(CALL)->pt)
#define COGO_CALL_FUNC(CALL)              ((CALL)->func)
#define COGO_CALL_CALLER(CALL)            ((CALL)->caller)
#define COGO_CALL_TOP(CALL)               ((CALL)->a.top)
#define COGO_CALL_SCHED(CALL)             ((CALL)->a.sched)
#define COGO_CALL_IS_VALID(CALL)          (COGO_PT_IS_VALID(COGO_CALL_PT(CALL)) && COGO_CALL_FUNC(CALL))
#define COGO_CALL_BEGIN(CALL)             COGO_DO_BEGIN(CALL, COGO_CALL_PT(CALL))
#define COGO_CALL_YIELD(CALL)             COGO_DO_YIELD(CALL, COGO_CALL_PT(CALL), COGO_ON_YIELD, COGO_ON_RESUME)
#define COGO_CALL_RETURN(CALL)            COGO_DO_RETURN(CALL, COGO_CALL_PT(CALL))
#define COGO_CALL_END(CALL)               COGO_DO_END(CALL, COGO_CALL_PT(CALL))
#define COGO_CALL_STATUS(CALL)            COGO_PT_STATUS(COGO_CALL_PT(CALL))
#define COGO_CALL_AWAIT(CALL, CALL_OTHER) COGO_DO_AWAIT(CALL, CALL, CALL_OTHER, CALL_OTHER)
#define COGO_CALL_RESUME                  cogo_call_resume

#define COGO_CALLSCHED_INIT(COGO) \
    {                             \
            /*.top=*/(COGO),      \
    }
#define COGO_CALLSCHED_TOP(CALLSCHED)      ((CALLSCHED)->top)
#define COGO_CALLSCHED_IS_VALID(CALLSCHED) ((COGO_CALLSCHED_TOP(CALLSCHED) && COGO_IS_VALID(COGO_CALLSCHED_TOP(CALLSCHED))) || !COGO_CALLSCHED_TOP(CALLSCHED))
#define COGO_CALLSCHED_RESUME              cogo_callsched_resume

#undef COGO_INIT
#undef COGO_PT_OF
#undef COGO_IS_VALID

#define COGO_INIT(COGO, FUNC)        COGO_CALL_INIT(COGO, FUNC)
#define COGO_CALL_OF(COGO)           (COGO)
#define COGO_PT_OF(COGO)             COGO_CALL_PT(COGO_CALL_OF(COGO))
#define COGO_FUNC_OF(COGO)           COGO_CALL_FUNC(COGO_CALL_OF(COGO))
#define COGO_CALLER_OF(COGO)         COGO_CALL_CALLER(COGO_CALL_OF(COGO))
#define COGO_TOP_OF(COGO)            COGO_CALL_TOP(COGO_CALL_OF(COGO))
#define COGO_SCHED_OF(COGO)          COGO_CALL_SCHED(COGO_CALL_OF(COGO))
#define COGO_IS_VALID(COGO)          ((COGO) == (COGO) && (COGO) && COGO_CALL_IS_VALID(COGO))

/// @hideinitializer Resume a coroutine.
/// @param[in] COGO The coroutine object pointer.
#define COGO_RESUME(COGO)            COGO_CALL_RESUME(COGO)

/// Run another coroutine until finished.
/// @pre `!__has_side_effects(COGO) && (COGO) != nullptr`
/// @pre `!__has_side_effects(COGO_OTHER) && (COGO_OTHER) != nullptr`
#define COGO_AWAIT(COGO, COGO_OTHER) COGO_DO_AWAIT(COGO, COGO_CALL_OF(COGO), COGO_OTHER, COGO_CALL_OF(COGO_OTHER))

/// @hideinitializer Initialize a scheduler object.
/// @param[in] COGO The coroutine object pointer.
/// @return A scheduler object initialized with the coroutine object.
#define COGO_SCHED_INIT(COGO)        COGO_CALLSCHED_INIT(COGO)
#define COGO_SCHED_CALLSCHED(SCHED)  (SCHED)
#define COGO_SCHED_IS_VALID(SCHED)   ((SCHED) == (SCHED) && (SCHED) && COGO_CALLSCHED_IS_VALID(SCHED))
#define COGO_SCHED_RESUME(SCHED)     COGO_CALLSCHED_RESUME(SCHED)
#define COGO_SCHED_TOP(SCHED)        COGO_CALLSCHED_TOP(COGO_SCHED_CALLSCHED(SCHED))

/// Abbr of `COGO_AWAIT(COGO_THIS, COGO_OTHER)`.
#define CO_AWAIT(COGO_OTHER)         COGO_AWAIT(COGO_THIS, COGO_OTHER)

#define COGO_DO_AWAIT(COGO, CALL, COGO_OTHER, CALL_OTHER)                                                   \
    do {                                                                                                    \
        COGO_BEFORE_AWAIT((+(COGO)), (+(COGO_OTHER)));                                                      \
                                                                                                            \
        COGO_CALL_CALLER(CALL_OTHER) = (COGO);                             /* call stack push */            \
        COGO_SCHED_TOP(COGO_CALL_SCHED(CALL)) = COGO_CALL_TOP(CALL_OTHER); /* continue from resume point */ \
        COGO_DO_YIELD(COGO, COGO_CALL_PT(CALL), COGO_NOOP, COGO_NOOP);                                      \
                                                                                                            \
        COGO_AFTER_AWAIT((+(COGO)), (+(COGO_OTHER)));                                                       \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif  // COGO_CALL_H_
