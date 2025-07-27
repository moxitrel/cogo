#include "cogo/cogo_call.h"

static COGO_T* cogo_call_sched_resume(COGO_SCHED_T* const sched) {
#define TOP        COGO_SCHED_TOP_OF(sched)
#define TOP_STATUS COGO_STATUS(TOP)
#define TOP_FUNC   COGO_FUNC_OF(TOP)
#define TOP_CALLER COGO_CALLER_OF(TOP)
#define TOP_SCHED  COGO_SCHED_OF(TOP)
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));

    while (TOP) {  // end
        COGO_ASSERT(COGO_IS_VALID(TOP));
        if (TOP_STATUS == COGO_STATUS_END) {  // awaited
            TOP = TOP_CALLER;
        } else {
            const COGO_T* const top0 = TOP;
            TOP_SCHED = sched;
            TOP_FUNC(TOP);
            if (TOP_STATUS > COGO_STATUS_BEGIN && TOP == top0) {  // yield
                break;
            }
        }
    }

    return TOP;

#undef TOP_SCHED
#undef TOP_CALLER
#undef TOP_FUNC
#undef TOP_STATUS
#undef TOP
}

// Run until CO_YIELD().
const COGO_T* cogo_call_resume(COGO_T* const cogo) {
    COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(cogo));    // Restore the resume point.
    return COGO_TOP_OF(cogo) = cogo_call_sched_resume(&sched);  // Save the resume point.
}
