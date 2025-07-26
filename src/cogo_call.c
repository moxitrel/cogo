#include <cogo/cogo_call.h>

static COGO_T* cogo_call_sched_resume(COGO_SCHED_T* const sched) {
#define TOP        COGO_SCHED_TOP_OF(sched)
#define TOP_STATUS COGO_STATUS(TOP)
#define TOP_FUN    COGO_FUN_OF(TOP)
#define TOP_CALLER COGO_CALLER_OF(TOP)
#define TOP_SCH    COGO_SCH_OF(TOP)
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));

    while (TOP) {  // end
        COGO_ASSERT(COGO_IS_VALID(TOP));
        if (TOP_STATUS == COGO_STATUS_END) {  // awaited
            TOP = TOP_CALLER;
        } else {
            COGO_T* top0 = TOP;
            TOP_SCH = sched;
            TOP_FUN(TOP);
            if (TOP_STATUS > COGO_STATUS_BEGIN && TOP == top0) {  // yield
                break;
            }
        }
    }

    return TOP;

#undef TOP_SCH
#undef TOP_CALLER
#undef TOP_FUN
#undef TOP_STATUS
#undef TOP
}

// Run until CO_YIELD().
COGO_T* cogo_call_resume(COGO_T* const cogo) {
    COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(cogo));    // Restore the resume point.
    return COGO_TOP_OF(cogo) = cogo_call_sched_resume(&sched);  // Save the resume point.
}
