#include <cogo/cogo_call.h>

// Run until CO_YIELD().
cogo_pc_t cogo_call_resume(cogo_call_t* const await) {
#define TOP         (sched.cogo_top)
#define TOP_SCHED   (COGO_CALL_OF(TOP)->cogo_union.sched)
#define TOP_AWAITER (COGO_CALL_OF(TOP)->cogo_caller)
#define TOP_FUNC    (COGO_CALL_OF(TOP)->cogo_func)
    COGO_ASSERT(cogo_call_is_valid(await));

    if (COGO_PC(await) != COGO_PC_END) {
        cogo_call_sched_t sched = COGO_CALL_SCHED_INIT(await->cogo_union.top);  // Restore the resume point.
        for (;;) {
            COGO_ASSERT(cogo_call_is_valid(TOP));
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
        await->cogo_union.top = TOP;  // Save the resume point.
    }

    return COGO_PC(await);
#undef TOP_FUNC
#undef TOP_AWAITER
#undef TOP_SCHED
#undef TOP
}
