#ifndef COROUTINE_AWAIT_H
#define COROUTINE_AWAIT_H

#include "gen.hpp"

// await_t: support call stack. (behave like function, slow about 10 stores when -O)
//  .step () -> await_t *: keep running until yield, return the next coroutine (call stack top) to be run.
//  .run  ()             : keep running until finish.
class await_t : protected gen_t {
    // the coroutine function
    virtual void operator()() = 0;
    // the next lower stack frame
    await_t *caller = nullptr;
protected:
    // the stack top of current call stack, one per scheduler
    thread_local static await_t *stack_top;

    // run the coroutine pointed by stack_top until yield
    static void stack_step()
    {
        assert(stack_top);

        if (stack_top->state() < 0) {
            // pop
            stack_top = stack_top->caller;
        } else {
            stack_top->operator()();
        }
    }

    void _await(await_t &callee)
    {
        // push
        callee.caller = this;
        stack_top = &callee;
    }
public:
    // run until yield, and return the next coroutine to be run. (for generator)
    // return NULL if coroutine finished
    await_t *step()
    {
        await_t *next = this;

        if (state() < 0) {
            // set caller as the new stack top, (pop)
            next = caller;
        } else {
            // clear stack_top
            if (stack_top != nullptr) {
                stack_top = nullptr;
            }

            operator()();
            if (stack_top != nullptr) {
                // set callee as the new stack top
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

thread_local await_t *await_t::stack_top;

// Call another coroutine. (await)
// await_t::co_await(await_t &co);
#define co_await(CO)                        \
do {                                        \
    await_t::_await(CO);                    \
    co_yield();                             \
} while (0)

#endif // COROUTINE_AWAIT_H
