#ifndef COGOTO_AWAIT_H
#define COGOTO_AWAIT_H

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

    // the child who called by me, (the new call stack top), temporarily store.
    struct await_t *stack_top;
} await_t;

#define AWAIT(FUN)   ((await_t){.fun = (void (*)(await_t *))(FUN),})

void await__await(await_t *co, await_t *callee)
{
    assert(co);
    assert(callee);

    callee->caller = co;        // (push)
    co->stack_top = callee;     // record the new stack top temporarily
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
        co->stack_top = NULL;   // clear
        co->fun(co);
        if (co->stack_top != NULL) {
            // set callee as the new stack top
            co = co->stack_top;
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
    await_t *_co = (await_t *)(CO);             \
    await__await(_co, (await_t *)(CALLEE));     \
    co_yield(_co);                              \
} while (0)

#endif // COGOTO_AWAIT_H
