#ifndef COROUTINE_CO_H
#define COROUTINE_CO_H

#include "await.hpp"

class co_t;
class co_queue_t;
typedef co_queue_t co_blocking_t;

// co_t queue
class co_queue_t {
    co_t *head = nullptr;   // empty queue
    co_t *tail;
public:
    bool empty() const;
    // dequeue, delete first node and return. If empty, return NULL.
    co_t *pop();
    // enqueue, append a coroutine to the end of queue.
    void push(co_t &);
    // join another co_queue_t to the end of queue.
    void append(co_queue_t &);
};

// co_t: support concurrency (slow about 24 stores when -O)
//  .state() -> int   : return the current running state.
//  .step () -> co_t *: run current coroutine until yield, return the next coroutine in the call stack.
//  .run  ()          : keep running until all coroutines finished.
class co_t : public await_t {
    friend co_queue_t;
    
    // scheduler related: q, next, await_t::stack_top, tmp_wait
    // concurrent coroutine queue (coroutines run concurrently).
    thread_local static co_queue_t q;
    // Temporarily store the coroutine blocked by blocking. Used by co_wait(), step()
    thread_local static co_t *tmp_wait;

    co_t *next;
protected:
    // only allow co_t (forbid await_t)
    void _await(co_t &co); /* hide await_t::_await() */

    void _sched(co_t &co);
    void _wait(co_queue_t &wq);
    void _broadcast(co_queue_t &wq);
public:
    // cast the return type only
    co_t *step();   /* hide await_t::step() */
    // keep running until all coroutines finished
    void run();     /* hide await_t::run() */
};

thread_local co_queue_t co_t::q;
thread_local co_t *co_t::tmp_wait = nullptr;

// await
#undef co_await
#define co_await(CO)            \
do {                            \
    co_t::_await(CO);           \
    co_yield();                 \
} while (0)

// Add a new coroutine CO to the scheduler. (create)
// co_sched(co_t &)
#define co_sched(CO)            \
do {                            \
    co_t::_sched(CO);           \
    co_yield();                 \
} while (0)

// Wait until notified.
// co_wait(co_queue_t &)
#define co_wait(Q)              \
do {                            \
    co_t::_wait(Q);             \
    co_yield();                 \
} while (0)

// Notify all coroutines blocked by Q.
// co_broadcast(co_queue_t &)
#define co_broadcast(Q)         \
do {                            \
    co_t::_broadcast(Q);        \
    co_yield();                 \
} while (0)


//
// co_queue_t
//
inline bool co_queue_t::empty() const
{
    return head == nullptr;
}

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
    if (empty()) {
        head = tail = &co;
    } else {
        tail->next = &co;
        tail = &co;
    }
    tail->next = nullptr;
}

inline void co_queue_t::append(co_queue_t &wq)
{
    if (!wq.empty()) {
        if (empty()) {
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
inline void co_t::_await(co_t &co)
{
    await_t::_await(co);
}

inline void co_t::_sched(co_t &co)
{
    q.push(co);
}

inline void co_t::_wait(co_queue_t &wq)
{
    wq.push(*this);
    co_t::tmp_wait = this;
}

inline void co_t::_broadcast(co_queue_t &wq)
{
    q.append(wq);
}

co_t *co_t::step()
{
    // cast ensured by _await()
    return (co_t *)await_t::step();
}

void co_t::run()
{
    q.push(*this);

//    for (co_t *co; (co = q.pop()) != nullptr; ) {
//        co = co->step();
//        if (tmp_wait != nullptr) {
//            tmp_wait = nullptr;
//            // remove call stack
//        } else if (co == nullptr) {
//            // remove call stack
//        } else {
//            q.push(*co);
//        }
//    }

    while ((await_t::stack_top = q.pop()) != nullptr) {
        await_t::stack_step();
        if (tmp_wait != nullptr) {
            tmp_wait = nullptr;
            // remove call stack
        } else if (await_t::stack_top == nullptr) {
            // remove call stack
        } else {
            // cast ensured by _await(), _sched()
            q.push(*(co_t *)await_t::stack_top);
        }
    }
}

#endif //COROUTINE_CO_H
