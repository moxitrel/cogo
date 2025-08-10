#include "cogo/cogo_call.h"

COGO_CALL_T* cogo_callsched_resume(COGO_CALLSCHED_T* const callsched) {
#define TOP        COGO_CALLSCHED_TOP(callsched)
#define TOP_STATUS COGO_STATUS(TOP)
#define TOP_FUNC   COGO_FUNC_OF(TOP)
#define TOP_CALLER COGO_CALLER_OF(TOP)
#define TOP_SCHED  COGO_SCHED_OF(TOP)
    COGO_ASSERT(COGO_CALLSCHED_IS_VALID(callsched));

    while (TOP) {  // end
        COGO_ASSERT(COGO_IS_VALID(TOP));
        if (TOP_STATUS == COGO_STATUS_END) {  // awaited
            TOP = TOP_CALLER;
        } else {
            const COGO_T* const top0 = TOP;
            TOP_SCHED = callsched;
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
const COGO_CALL_T* cogo_call_resume(COGO_CALL_T* const call) {
    COGO_ASSERT(COGO_CALL_IS_VALID(call));
    COGO_CALLSCHED_T callsched = COGO_CALLSCHED_INIT(COGO_CALL_TOP(call));  // Restore the resume point.
    return COGO_CALL_TOP(call) = COGO_CALLSCHED_RESUME(&callsched);         // Save the resume point.
}
