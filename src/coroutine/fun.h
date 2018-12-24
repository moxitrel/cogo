#ifndef COROUTINE_FUN_H
#define COROUTINE_FUN_H

#include "gen.h"
#include <stddef.h>

// fun_t: support call stack.
typedef struct fun_t {
    gen_t gen;

    // the coroutine function
    void (*const fun)(struct fun_t *);

    // The parent who call me. (call stack)
    struct fun_t *caller;
} fun_t;

#define FUN(F)   ((fun_t){.fun = (void (*)(fun_t *))(F),})

// Temporarily store the coroutine who called by me. Used by co_call()
_Thread_local static const fun_t *fun_tmp_callee;

// Run the coroutine until yield.
fun_t *fun_step(fun_t *co)
{
    assert(co);

    if (co_state(&co->gen) < 0) {
        co = co->caller;
    } else {
        co->fun(co);
        if (fun_tmp_callee != NULL) {
            co = (fun_t *)fun_tmp_callee;
            fun_tmp_callee = NULL;
        }
    }

    return co;
}

// Run the coroutine until finished.
void fun_run(fun_t *co)
{
    while (co != NULL) {
        co = fun_step(co);
    }
}

// Call another coroutine.
// co_call(co_t *, co_t *);
#define co_call(CO, CALLEE)                     \
do {                                            \
    fun_t *const _co = (fun_t *)(CO);           \
    fun_t *const _callee = (fun_t *)(CALLEE);   \
    _callee->caller = _co;                      \
    fun_tmp_callee = _callee;                   \
    co_return(_co);                             \
} while (0)

#endif // COROUTINE_FUN_H
