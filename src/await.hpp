#ifndef COGOTO_AWAIT_H
#define COGOTO_AWAIT_H

#include "gen.hpp"

// await_t: add call stack support, behave like function
//  .state() -> int      : return the current running state.
//  .step () -> await_t *: keep running until yield, return the next coroutine (call stack top) to be run.
//  .run  ()             : keep running until finish.
class await_t : public gen_t {
    // the coroutine function
    virtual void operator()() = 0;

    // the lower call stack frame
    await_t *caller = nullptr;

    // the stack top of current call stack
    // NOTE: the implementation of TLS is too slow!!!
    /* thread_local static */ await_t *stack_top;
protected:
    // call another coroutine
    void _await(await_t &callee)
    {
        // stack push
        callee.caller = this;
        stack_top = &callee;
    }
public:
    // run until yield, return the new call stack top or NULL if finished
    await_t *step()
    {
        stack_top = this;

        if (state() < 0) {
            // stack pop
            stack_top = caller;
        } else {
            // stack top may be changed by await()
            operator()();
        }

        return stack_top;
    }

    // keep running until finish
    void run()
    {
        for (await_t *co = this; co != nullptr; ) {
            co = co->step();
        }
    }
};

// Call another coroutine. (await)
// await_t::co_await(await_t &co);
#define co_await(CO)                        \
do {                                        \
    _await(CO);                             \
    co_yield();                             \
} while (0)

#endif // COGOTO_AWAIT_H
