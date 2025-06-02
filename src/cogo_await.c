#include <cogo/cogo_await.h>

// Run until CO_YIELD().
cogo_await_t const* cogo_await_resume(cogo_await_t* const await) {
    COGO_ASSERT(COGO_IS_VALID(await));
    if (COGO_PC(await) != COGO_PC_END) {
        COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(await));  // Restore the resume point.
#define TOP         COGO_SCHED_TOP_OF(&sched)
#define TOP_PC      COGO_PC(TOP)
#define TOP_FUNC    COGO_FUNC_OF(TOP)
#define TOP_AWAITER COGO_AWAITER_OF(TOP)
#define TOP_SCHED   COGO_SCHED_OF(TOP)

        for (;;) {
            COGO_ASSERT(COGO_IS_VALID(TOP));
            TOP_SCHED = &sched;
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
    exit:
        COGO_TOP_OF(await) = TOP;  // Save the resume point.

#undef TOP_SCHED
#undef TOP_AWAITER
#undef TOP_FUNC
#undef TOP_PC
#undef TOP
    }

    return COGO_TOP_OF(await);
}
