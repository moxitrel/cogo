#ifndef COROUTINE_AWAIT_H
#define COROUTINE_AWAIT_H

#include "gen.hpp"

// await_t: support call stack. (behave like function, slow about 10 stores when -O)
//  .state() -> int      : return the current running state.
//  .step () -> await_t *: keep running until yield, return the next coroutine (call stack top) to be run.
//  .run  ()             : keep running until finish.
class await_t : public gen_t {
    // the coroutine function
    virtual void operator()() = 0;
    // the next lower stack frame
    await_t *caller = nullptr;
protected:
    // the stack top of current call stack
    thread_local static await_t *stack_top;

    // run the coroutine pointed by stack_top until yield, and refresh the call stack top
    static void stack_step()
    {
        assert(stack_top);

        if (stack_top->state() < 0) {
            // stack pop
            stack_top = stack_top->caller;
        } else {
            stack_top->operator()();
        }
    }

    void _await(await_t &callee)
    {
        // stack push
        callee.caller = this;
        stack_top = &callee;
    }
public:
    // behave the same as stack_step(), provide for generator
    // return NULL if finished
    await_t *step()
    {
        await_t *next = this;

        if (state() < 0) {
            // set caller as the new stack top, (pop)
            next = caller;
        } else {
            // use stack_top as a temporary storage for callee if exist
            // require stack_top inited to NULL, and clear when exit. (to improve performance)
            operator()();
            if (stack_top != nullptr) {
                // set callee as the new stack top, (push)
                next = stack_top;
                stack_top = nullptr;
            }
        }

        return next;
    }
    
    // keep running until finish
    void run()
    {
        for (stack_top = this; stack_top != nullptr;) {
            stack_step();
        }
    }
};

// step() require stack_top inited with NULL
thread_local await_t *await_t::stack_top = nullptr;

// Call another coroutine. (await)
// await_t::co_await(await_t &co);
#define co_await(CO)                        \
do {                                        \
    await_t::_await(CO);                    \
    co_yield();                             \
} while (0)

#endif // COROUTINE_AWAIT_H
