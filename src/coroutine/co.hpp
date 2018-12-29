#ifndef COROUTINE_CO_H
#define COROUTINE_CO_H

#include "fun.hpp"

class co_t;
class co_queue_t;
typedef co_queue_t co_blocking_t;

// co_t queue
class co_queue_t {
    co_t *head = nullptr;   // empty queue
    co_t *tail;
public:
    // dequeue, delete first node and return. If empty, return NULL.
    co_t *pop();
    // enqueue, append a coroutine to the end of queue.
    void push(co_t &);
    // join another co_queue_t to the end of queue.
    void append(co_queue_t &);
};

// co_t: support concurrency (slow about 8 stores when -O)
//  .step () -> co_t *: run current coroutine until yield, return the next coroutine in the call stack.
//  .run  ()          : keep running until all coroutines finished.
class co_t : protected fun_t {
    friend co_queue_t;
    
    // Scheduler related: q, next, tmp_wait
    // Concurrent coroutine queue (coroutines run concurrently).
    inline thread_local static co_queue_t q;
    co_t *next;
    // Temporarily store the coroutine blocked by blocking. Used by co_wait(), step()
    inline thread_local static co_t *tmp_wait = nullptr;
protected:
    // Only allow co_t. (forbid fun_t)
    void _call(co_t &callee) /* hide fun_t::_call() */
    {
        fun_t::_call(callee);
    }
    void _sched(co_t &co)
    {
        q.push(co);
    }
    void _wait(co_queue_t &wq)
    {
        wq.push(*this);
        co_t::tmp_wait = this;
    }
    void _broadcast(co_queue_t &wq)
    {
        q.append(wq);
    }
public:
    // Run until yield. Return call stack top, or NULL if finished or blocked.
    co_t *step(); /* hide fun_t::step() */
    // Keep running until all coroutines finished.
    void run(); /* hide fun_t::step() */
};

// await
#undef co_call
#define co_call(CO)             \
do {                            \
    co_t::_call(CO);            \
    co_return();                \
} while (0)

// Add a new coroutine CO to the scheduler. (create)
// co_sched(co_t &)
#define co_sched(CO)            \
do {                            \
    co_t::_sched(CO);           \
    co_return();                \
} while (0)

// Wait until notified.
// co_wait(co_queue_t &)
#define co_wait(Q)              \
do {                            \
    co_t::_wait(Q);             \
    co_return();                \
} while (0)

// Notify all coroutines blocked by Q.
// co_broadcast(co_queue_t &)
#define co_broadcast(Q)         \
do {                            \
    co_t::_broadcast(Q);        \
    co_return();                \
} while (0)

//
// co_queue_t
//

inline co_t *co_queue_t::pop()
{
    co_t *node = head;
    if (head != nullptr) {
        head = head->next;
    }
    return node;
}

inline void co_queue_t::push(co_t &co)
{
    if (head == nullptr) {
        head = tail = &co;
    } else {
        tail->next = &co;
        tail = &co;
    }
    tail->next = nullptr;
}

inline void co_queue_t::append(co_queue_t &wq)
{
    if (wq.head != nullptr) {
        if (head == nullptr) {
            *this = wq;
        } else {
            tail->next = wq.head;
            tail = wq.tail;
        }
    }
}

//
// co_t
//
co_t *co_t::step()
{
    co_t *next = (co_t *)fun_t::step(); // cast ensured by _call(co_t &)
    if (tmp_wait != nullptr) {
        // delete from coroutine queue if blocked
        next = nullptr;
        tmp_wait = nullptr;
    }
    return next;
}

void co_t::run()
{
    q.push(*this);
    for (co_t *node; (node = q.pop()) != nullptr;) {
        node = node->step();
        if (node != nullptr) {
            q.push(*node);
        }
    }
}

#endif //COROUTINE_CO_H
