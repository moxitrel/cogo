/*

* API  (!!! not type safe !!!)
- co_await(await_t *, await_t *)    :: call another coroutine.

- await_t AWAIT(void(*)(await_t *)) :: await_t constructor.
- await_run(await_t *)              :: run a coroutine until finish.

*/
#ifndef COGO_AWAIT_H
#define COGO_AWAIT_H

#include "gen.h"

typedef struct await_t     await_t;
typedef struct await_sch_t await_sch_t;

// await_t: gen_t with call stack support.
struct await_t {
    // inherit gen_t
    gen_t gen_t;

    // the coroutine function
    void (*fun)(await_t *);

    // the lower call stack frame (who call me)
    await_t *caller;

    // scheduler, inited by co_await() or await_run()
    await_sch_t *sch;
};

// await_t scheduler
struct await_sch_t {
    // call stack top, the coroutine run by scheduler
    await_t *stack_top;
};

// await_t AWAIT(void(*)(await_t *): await_t constructor, **sch isn't inited**.
#define AWAIT(FUN) ((await_t){          \
    .fun = (void(*)(await_t *))(FUN),   \
})

// await_sch_t AWAIT_SCH(await_t *): await_sch_t constructor
#define AWAIT_SCH(AWAIT)    ((await_sch_t){.stack_top = (await_t *)(AWAIT),})


// push callee to call stack
inline static await_t *await_call(await_t *self, await_t *callee)
{
    assert(self);
    assert(self->sch);
    assert(callee);

    // call stack push
    callee->caller = self;
    callee->sch    = self->sch;
    self->sch->stack_top = callee;  // set new stack top

    return self;
}
// co_await(await_t *, await_t *): call another coroutine.
#define co_await(AWAIT, CALLEE)     co_yield(await_call((await_t *)(AWAIT), (await_t *)(CALLEE)))

// run the coroutine at stack top until yield
inline static void await_sch_step(await_sch_t *sch)
{
    assert(sch);
    assert(sch->stack_top);

    if (co_state(sch->stack_top) < 0) {
        // call stack pop
        sch->stack_top = sch->stack_top->caller;
    } else {
        // run
        sch->stack_top->fun(sch->stack_top);
    }
}



// run the coroutine until finish.
inline static void await_run(void *await)
{
    assert(await);
    await_sch_t sch = {
        .stack_top = (await_t *)await,
    };
    sch.stack_top->sch = &sch;

    for (;sch.stack_top;) {
        await_sch_step(&sch);
    }
}

//inline static await_t *await_step(await_t *self)
//{
//    assert(self);
//
//    await_sch_t sch;
//    if (!self->sch) {
//        self->sch = &sch;
//    }
//
//    for (;self;) {
//        self->sch->stack_top = self;
//        self->fun(self);
//        if (co_state(self) < 0) {
//            self = self->caller;
//        } else if (self->sch->stack_top != self) {
//            // co_await() called
//            self = self->sch->stack_top;
//            continue;
//        } else {
//            // co_yield() called
//            break;
//        }
//    }
//
//    return self;
//}

#endif // COGO_AWAIT_H
