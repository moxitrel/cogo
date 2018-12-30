#ifndef COROUTINE_AWAIT_H
#define COROUTINE_AWAIT_H

#include "gen.h"
#include <stddef.h>

// await_t: support call stack.
typedef struct await_t {
    // inherit gen_t
    gen_t gen;

    // the coroutine function
    void (*const fun)(struct await_t *);

    // The parent who call me. (build call stack)
    struct await_t *caller;
} await_t;

// the child who called by me, (the new call stack top), temporarily store.
_Thread_local static await_t *await__tmp_callee;

#define AWAIT(FUN)   ((await_t){.fun = (void (*)(await_t *))(FUN),})

void await__await(await_t *co, await_t *callee)
{
    assert(co);
    assert(callee);

    callee->caller = co;            // (push)
    await__tmp_callee = callee;     // record the new stack top temporarily
}

// Run until yield, and return the call stack top.
// Return NULL if coroutine finished.
await_t *await_step(await_t *co)
{
    assert(co);

    if (co_state(&co->gen) < 0) {
        // set caller as the new stack top, (pop)
        co = co->caller;
    } else {
        co->fun(co);
        if (await__tmp_callee != NULL) {
            // set callee as the new stack top
            co = await__tmp_callee;
            await__tmp_callee = NULL;   // clear
        }
    }

    return co;
}

// Run the coroutine until finished.
void await_run(await_t *co)
{
    while (co != NULL) {
        co = await_step(co);
    }
}

// Call another coroutine. (await)
// co_await(await_t *, await_t *);
#define co_await(CO, CALLEE)                    \
do {                                            \
    await__await((CO), (CALLEE));               \
    co_yield(_co);                              \
} while (0)

#endif // COROUTINE_AWAIT_H
