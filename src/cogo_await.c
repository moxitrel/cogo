#include <cogo/cogo_await.h>

COGO_T const* cogo_await_sched_resume(COGO_SCHED_T* sched) {
#define TOP         COGO_SCHED_TOP_OF(sched)
#define TOP_PC      COGO_PC(TOP)
#define TOP_FUNC    COGO_FUNC_OF(TOP)
#define TOP_AWAITER COGO_AWAITER_OF(TOP)
#define TOP_SCHED   COGO_SCHED_OF(TOP)
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));

    if (TOP) {
        for (;;) {
            COGO_ASSERT(COGO_IS_VALID(TOP));
            TOP_SCHED = sched;
            TOP_FUNC(TOP);
            switch (TOP_PC) {
                case COGO_PC_END:
                    if (!(TOP = TOP_AWAITER)) {  // end
                        goto exit;
                    }
                    continue;        // awaited
                case COGO_PC_BEGIN:  // awaiting
                    continue;
                default:  // yield
                    goto exit;
            }
        }
    }
exit:
    return TOP;  // Save the resume point.

#undef TOP_SCHED
#undef TOP_AWAITER
#undef TOP_FUNC
#undef TOP_PC
#undef TOP
}

// Run until CO_YIELD().
COGO_T const* cogo_await_resume(COGO_T* const await) {
    COGO_ASSERT(COGO_IS_VALID(await));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(await));  // Restore the resume point.
    cogo_await_sched_resume(&sched);
    return COGO_TOP_OF(await) = COGO_SCHED_TOP_OF(&sched);  // Save the resume point.
}
