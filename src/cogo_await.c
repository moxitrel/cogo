#include <cogo/cogo_await.h>

// Should be invoked through CO_AWAIT().
void cogo_await_await(cogo_await_t* const await, cogo_await_t* const awaitee) {
#define TOP (await->anon.sched->top)

#ifdef COGO_DEBUG
    cogo_await_t const* node;
    // No loop in the call chain.
    for (node = await; node; node = node->awaiter) {
        COGO_ASSERT(cogo_other != node);
    }
#endif
    COGO_ASSERT(cogo_await_is_valid(cogo_this) && cogo_this->anon.sched && cogo_await_is_valid(cogo_other));

    awaitee->awaiter = TOP;   // call stack push
    TOP = awaitee->anon.top;  // continue from resume point

#undef TOP
}

// Run until CO_YIELD().
cogo_pc_t cogo_await_resume(cogo_await_t* const await) {
#define TOP         (sched.top)
#define TOP_SCHED   (COGO_AWAIT_OF(TOP)->anon.sched)
#define TOP_AWAITER (COGO_AWAIT_OF(TOP)->awaiter)
#define TOP_FUNC    (COGO_AWAIT_OF(TOP)->func)
    COGO_ASSERT(cogo_await_is_valid(await));

    if (COGO_PC(await) != COGO_PC_END) {
        cogo_await_sched_t sched = COGO_AWAIT_SCHED_INIT(await->anon.top);  // Restore the resume point.
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
        await->anon.top = TOP;  // Save the resume point.
    }

    return COGO_PC(await);
#undef TOP_FUNC
#undef TOP_AWAITER
#undef TOP_SCHED
#undef TOP
}

void cogo_await_run(cogo_await_t* const await) {
    COGO_ASSERT(cogo_await_is_valid(await));
    while (cogo_await_resume(await) != COGO_PC_END) {
    }
}
