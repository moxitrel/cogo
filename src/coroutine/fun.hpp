#ifndef COROUTINE_FUN_H
#define COROUTINE_FUN_H

#include "gen.hpp"

// fun_t: support call stack.
//  .state(): return the running state.
//  .step() : run the coroutine until yield.
//  .run()  : run the coroutine until finished.
class fun_t : public gen_t {
    // the coroutine function
    virtual void operator()() = 0;

    // The parent who call me. (call stack)
    fun_t *caller = nullptr;

    // Temporarily store the coroutine who called by me. Used by co_call()
    inline thread_local static fun_t *tmp_callee = nullptr;
protected:
    // Push callee into call stack.
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
            operator()();
            if (tmp_callee != nullptr) {
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
    fun_t::_call(CALLEE);                   \
    co_return()


#endif // COROUTINE_FUN_H
