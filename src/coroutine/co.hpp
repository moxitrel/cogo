#ifndef COROUTINE_CO_H
#define COROUTINE_CO_H

#include "fun.hpp"

class co_t;
class co_queue_t;
typedef co_queue_t co_blocking_t;

// co_t queue
class co_queue_t {
    co_t *head = nullptr;
    co_t *tail;
public:
    // dequeue, delete first node and return. If empty, return NULL.
    co_t *pop();
    // enqueue, append ^co to the end of queue.
    void push(co_t &co);
    // join q to the end of queue.
    void join(co_queue_t &q);
};

// co_t: support concurrency
//  .step () -> co_t *: run current coroutine until yield, return the next coroutine in the call stack.
//  .run  ()          : keep running until all coroutines finished.
class co_t : protected fun_t {
    friend co_queue_t;

    // Scheduler related: _q, q_next, tmp_blocking
    co_t *q_next = nullptr;
protected:
    // Concurrent coroutine queue (coroutines run concurrently).
    inline thread_local static co_queue_t _q;

    // Temporarily store the coroutine blocked by blocking. Used by co_blocking_wait(), step()
    inline thread_local static co_t *tmp_blocking;

    // Only allow co_t, forbid fun_t. Used by co_call(), don't invoke it elsewhere.
    void _call(co_t &callee) /* hide fun_t::_call() */
    {
        fun_t::_call(callee);
    }
public:
    co_t *step() /* hide fun_t::step() */
    {
        tmp_blocking = nullptr;
        co_t *next = (co_t *)fun_t::step(); // cast ensured by _call(co_t &)
        if (tmp_blocking != nullptr) {
            // delete from coroutine queue if blocked
            next = nullptr;
        }
        return next;
    }

    // Keep running until all coroutines finished.
    void run() /* hide fun_t::step() */
    {
        _q.push(*this);
        for (co_t *node; (node = _q.pop()) != nullptr;) {
            node = node->step();
            if (node != nullptr) {
                _q.push(*node);
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
    co_t::_q.push(CO);          \
    co_return()

// co_blocking_wait(co_blocking_t &)
#define co_wait(Q)              \
    (Q).push(*this);            \
    co_t::tmp_blocking = this;  \
    co_return()                 \

#define co_broadcast(Q)         \
    co_t::_q.join(Q);           \
    co_return()


//
// co_queue_t
//

// dequeue, delete first node and return. If empty, return NULL.
inline co_t *co_queue_t::pop()
{
    co_t *node = head;
    if (head != nullptr) {
        head = head->q_next;
    }
    return node;
}

// enqueue, append ^co to the end of queue.
inline void co_queue_t::push(co_t &co)
{
    if (head == nullptr) {
        head = tail = &co;
    } else {
        tail->q_next = &co;
        tail = &co;
    }
    tail->q_next = nullptr;
}

// join q to the end of queue.
inline void co_queue_t::join(co_queue_t &q)
{
    if (head == nullptr) {
        *this = q;
    } else {
        if (q.head != nullptr) {
            tail->q_next = q.head;
            tail = q.tail;
        }
    }
}

#endif //COROUTINE_CO_H
