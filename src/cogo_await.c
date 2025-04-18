#include <cogo/cogo_await.h>

// Should be invoked through CO_AWAIT().
void cogo_await_await(cogo_await_t* const cogo_this, cogo_await_t* const cogo_other) {
#define TOP (cogo_this->anon.sched->top)

#ifdef COGO_DEBUG
    cogo_await_t const* node;
    // No loop in the call chain.
    for (node = cogo_this; node; node = node->caller) {
        COGO_ASSERT(cogo_other != node);
    }
#endif
    COGO_ASSERT(cogo_await_is_valid(cogo_this) && cogo_this->anon.sched && cogo_await_is_valid(cogo_other));

    cogo_other->caller = TOP;    // call stack push
    TOP = cogo_other->anon.top;  // continue from resume point

#undef TOP
}

// Run until CO_YIELD().
cogo_pc_t cogo_await_resume(cogo_await_t* const cogo) {
#define TOP        (sched.top)
#define TOP_SCHED  (COGO_AWAIT_OF(TOP)->anon.sched)
#define TOP_CALLER (COGO_AWAIT_OF(TOP)->caller)
#define TOP_FUNC   (COGO_YIELD_OF(TOP)->func)
    COGO_ASSERT(cogo_await_is_valid(cogo));

    if (COGO_PC(cogo) != COGO_PC_END) {
        cogo_await_sched_t sched = COGO_AWAIT_SCHED_INIT(cogo->anon.top);  // Restore the resume point.
        for (;;) {
            COGO_ASSERT(cogo_await_is_valid(TOP));
            TOP_SCHED = &sched;
            TOP_FUNC(TOP);
            switch (COGO_PC(TOP)) {
                case COGO_PC_END:
                    if (!(TOP = TOP_CALLER)) {  // end
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
        cogo->anon.top = TOP;  // Save the resume point.
    }

    return COGO_PC(cogo);
#undef TOP_FUNC
#undef TOP_CALLER
#undef TOP_SCHED
#undef TOP
}

void cogo_await_run(cogo_await_t* const cogo) {
    COGO_ASSERT(cogo_await_is_valid(cogo));
    while (cogo_await_resume(cogo) != COGO_PC_END) {
    }
}
