#include <cogo/cogo_call.h>

COGO_T const* cogo_call_sched_resume(COGO_SCHED_T* sched) {
#define TOP        COGO_SCHED_TOP_OF(sched)
#define TOP_PC     COGO_STATUS(TOP)
#define TOP_FUNC   COGO_FUNC_OF(TOP)
#define TOP_CALLER COGO_CALLER_OF(TOP)
#define TOP_SCHED  COGO_SCHED_OF(TOP)
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));

    while (TOP) {
        COGO_ASSERT(COGO_IS_VALID(TOP));
        TOP_SCHED = sched;
        TOP_FUNC(TOP);
        switch (TOP_PC) {
            case COGO_STATUS_END:  // awaited | end
                TOP = TOP_CALLER;
                continue;
            case COGO_STATUS_BEGIN:  // awaiting
                continue;
            default:  // yield
                goto exit;
        }
    }
exit:
    return TOP;

#undef TOP_SCHED
#undef TOP_CALLER
#undef TOP_FUNC
#undef TOP_PC
#undef TOP
}

// Run until CO_YIELD().
COGO_T const* cogo_call_resume(COGO_T* const cogo) {
    COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(cogo));  // Restore the resume point.
    COGO_SCHED_RESUME(&sched);
    return COGO_TOP_OF(cogo) = COGO_SCHED_TOP_OF(&sched);  // Save the resume point.
}
