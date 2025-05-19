#include <cogo/cogo_await.h>

// Run until CO_YIELD().
cogo_pc_t cogo_await_resume(cogo_await_t* const await) {
#define TOP         (sched.top)
#define TOP_SCHED   (COGO_AWAIT_OF(TOP)->a.sched)
#define TOP_AWAITER (COGO_AWAIT_OF(TOP)->awaiter)
#define TOP_FUNC    (COGO_AWAIT_OF(TOP)->func)
    COGO_ASSERT(cogo_await_is_valid(await));

    if (COGO_PC(await) != COGO_PC_END) {
        cogo_await_sched_t sched = COGO_AWAIT_SCHED_INIT(await->a.top);  // Restore the resume point.
        for (;;) {
            COGO_ASSERT(cogo_await_is_valid(TOP));
            TOP_SCHED = &sched;
            TOP_FUNC(TOP);
            switch (COGO_PC(TOP)) {
                case COGO_PC_END:
                    if (!(TOP = TOP_AWAITER)) {  // end
                        goto exit;
                    }
                    continue;        // awaited
                case COGO_PC_BEGIN:  // awaiting
                    continue;
                default:  // yielding
                    goto exit;
            }
        }
    exit:
        await->a.top = TOP;  // Save the resume point.
    }

    return COGO_PC(await);
#undef TOP_FUNC
#undef TOP_AWAITER
#undef TOP_SCHED
#undef TOP
}
