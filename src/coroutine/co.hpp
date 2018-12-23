#ifndef COROUTINE_CO_HPP
#define COROUTINE_CO_HPP

#include "fun.hpp"

// co_t:
//  .run()
class co_t : protected fun_t {
    // Coroutine queue, who run concurrently with me.
    inline thread_local static co_t *q_head;
    inline thread_local static co_t *q_tail;
    co_t *q_next = nullptr;
protected:
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
    void run()
    {
        q_tail = q_head = this;
        while (q_head != nullptr) {
            for (co_t **node = &q_head; *node != nullptr;) {
                co_t **node_next = &(*node)->q_next;

                co_t *co = *node;
                co = (co_t *) co->step();
                if (co == nullptr) {
                    *node = *node_next;
                } else {
                    *node = co;
                }

                node = node_next;
            }
        }
    }
};

#undef co_call
#define co_call(CALLEE)         \
    co_t::_call(CALLEE);        \
    co_return()


#define co_sched(CO)            \
    co_t::_sched(CO);           \
    co_return()

#endif //COROUTINE_CO_HPP
