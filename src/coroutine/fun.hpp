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
    // Temporarily store the coroutine who called by me (the new call stack top), used by co_call() and step()
    inline thread_local static fun_t *tmp_callee;
protected:
    // Push callee to call stack.
    void _call(fun_t &callee)
    {
        callee.caller = this;
        tmp_callee = &callee;
    }
public:
    // Run until yield. Return the next coroutine to be run, or NULL if no more coroutine exist.
    fun_t *step()
    {
        fun_t *next = this;

        if (state() < 0) {
            next = caller;
        } else {
            tmp_callee = nullptr;
            operator()();
            if (tmp_callee != nullptr) {
                next = tmp_callee;
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
    fun_t::_call(CALLEE);                   \
    co_return()


#endif // COROUTINE_FUN_H
