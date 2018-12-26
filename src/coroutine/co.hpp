#ifndef COROUTINE_CO_H
#define COROUTINE_CO_H

#include "fun.hpp"

// co_t: support concurrency
//  .step () -> co_t *: run current coroutine until yield, return the next coroutine in the call stack.
//  .run  ()          : keep running until all coroutines finished.
class co_t : protected fun_t {
    // Concurrent coroutine queue (coroutines run concurrently).
    inline thread_local static co_t *q_head;
    inline thread_local static co_t *q_tail;

    co_t *q_next = nullptr;
protected:
    // Only allow co_t, exclude fun_t
    void _call(co_t &callee) /* overwrite */
    {
        fun_t::_call(callee);
    }

    // Append co to the coroutine queue.
    void _sched(co_t &co)
    {
        q_tail->q_next = &co;
        q_tail = &co;
    }
public:
    co_t *step() /* overwrite */
    {
        // Ensured by _call(co_t &)
        return (co_t *)fun_t::step();
    }

    void run() /* overwrite */
    {
        for (q_tail = q_head = this; q_head != nullptr;) {
            co_t **node_next;
            for (co_t **node = &q_head; *node != nullptr; node = node_next) {
                node_next = &(*node)->q_next;

                *node = (*node)->step();
                // delete node if finished.
                if (*node == nullptr) {
                    *node = *node_next;
                }
            }
        }
    }
};

// await
#undef co_call
#define co_call(CALLEE)         \
    co_t::_call(CALLEE);        \
    co_return()

// Add a new coroutine CO to the scheduler. (create)
#define co_sched(CO)            \
    co_t::_sched(CO);           \
    co_return()

#endif //COROUTINE_CO_H
