#include <cogo/cogo_await.h>

COGO_T const* cogo_await_sched_resume(COGO_SCHED_T* sched) {
#define TOP         COGO_SCHED_TOP_OF(sched)
#define TOP_PC      COGO_PC(TOP)
#define TOP_FUNC    COGO_FUNC_OF(TOP)
#define TOP_AWAITER COGO_AWAITER_OF(TOP)
#define TOP_SCHED   COGO_SCHED_OF(TOP)
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));

    while (TOP) {
        COGO_ASSERT(COGO_IS_VALID(TOP));
        TOP_SCHED = sched;
        TOP_FUNC(TOP);
        switch (TOP_PC) {
            case COGO_PC_END:  // awaited | end
                TOP = TOP_AWAITER;
                continue;
            case COGO_PC_BEGIN:  // awaiting
                continue;
            default:  // yield
                goto exit;
        }
    }
exit:
    return TOP;

#undef TOP_SCHED
#undef TOP_AWAITER
#undef TOP_FUNC
#undef TOP_PC
#undef TOP
}

// Run until CO_YIELD().
COGO_T const* cogo_await_resume(COGO_T* const cogo) {
    COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(cogo));  // Restore the resume point.
    COGO_SCHED_RESUME(&sched);
    return COGO_TOP_OF(cogo) = COGO_SCHED_TOP_OF(&sched);  // Save the resume point.
}
