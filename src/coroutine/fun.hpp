#ifndef COROUTINE_FUN_H
#define COROUTINE_FUN_H

#include "gen.hpp"

// fun_t: support call stack.
//  .step () -> fun_t *: run until yield, return the next coroutine to be run.
//  .run  ()           : run until finished.
class fun_t : protected gen_t {
    // The coroutine function
    virtual void operator()() = 0;
    // The parent who call me. (build call stack)
    fun_t *caller = nullptr;
    // Store the coroutine who called by me temporarily (the new call stack top), used by co_call() and step().
    inline thread_local static fun_t *tmp_callee = nullptr;
protected:
    void _call(fun_t &callee)
    {
        callee.caller = this;
        tmp_callee = &callee;
    }
public:
    // Run until yield. Return the next coroutine to be run, or NULL if finished.
    fun_t *step()
    {
        fun_t *next = this;

        if (state() < 0) {
            // set caller as the new stack top
            next = caller;
        } else {
            operator()();
            if (tmp_callee != nullptr) {
                // set callee as the new stack top
                next = tmp_callee;
                tmp_callee = nullptr;
            }
        }

        return next;
    }

    // Keep running until finish.
    void run()
    {
        for (fun_t *co = this; co != nullptr;) {
            co = co->step();
        }
    }
};

// Call another coroutine. (await)
// fun_t::co_call(fun_t &callee);
#define co_call(CALLEE)                     \
do {                                        \
    fun_t::_call(CALLEE);                   \
    co_return();                            \
} while (0)

#endif // COROUTINE_FUN_H
