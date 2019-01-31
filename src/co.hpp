#ifndef COGO_CO_H
#define COGO_CO_H

#include "await.hpp"

class co_t;
class co_queue_t;
class sch_t;

typedef co_queue_t co_block_t;


// co_t: support concurrency
//  .state() -> int   : return the current running state.
//  .step () -> co_t *: run *current coroutine* until yield, return the next coroutine in the call stack.
//  .run  ()          : keep running until all coroutines finished
class co_t : public await_t {
    friend co_queue_t;
    friend sch_t;

    // associated scheduler
    sch_t *sch;
    // another concurrent coroutine in the co_queue_t
    co_t *next;
protected:
    void _await(co_t &co); /* hide await_t::_await() */
    void _sched(co_t &co);
    void _wait(co_block_t &);
    void _broadcast(co_block_t &);
public:
    co_t *step() /* hide await_t::step() */
    {
        // cast return type only, ensured by _await()
        return (co_t *)await_t::step();
    }

    // run until finish all
    void run();
};

// Add a new coroutine CO to the scheduler. (create)
// co_sched(co_t &)
#define co_sched(CO)            \
do {                            \
    _sched(CO);                 \
    co_yield();                 \
} while (0)

// Wait until notified.
// co_wait(co_block_t &)
#define co_wait(Q)              \
do {                            \
    co_t::_wait(Q);             \
    co_yield();                 \
} while (0)

// Notify all coroutines blocked by Q.
// co_broadcast(co_block_t &)
#define co_broadcast(Q)         \
do {                            \
    co_t::_broadcast(Q);        \
    co_yield();                 \
} while (0)


// coroutine queue
class co_queue_t {
    co_t *first = nullptr;   // empty queue
    co_t *last;
public:
    bool empty() const;

    // dequeue, delete first node and return. If empty, return NULL.
    co_t *pop();

    // enqueue, append a coroutine to the end of queue.
    void push(co_t &);

    // join another co_queue_t to the end of queue.
    void append(co_queue_t &);
};


// coroutine scheduler
class sch_t {
    friend co_t;

    // coroutines in this queue run concurrently
    co_queue_t q = {};

    // temporarily store the coroutine blocked by co_block_t. Used by co_wait(), step()
    co_t *blocked_coroutine = nullptr;
public:
    // run until finish all
    void run(co_t &);

    // run until finish all
    void run(co_t &&entry)
    {
        run(entry);
    }
};


//
// co_t
//

// only allow co_t (forbid await_t)
inline void co_t::_await(co_t &co)
{
    await_t::_await(co);
}

inline void co_t::_sched(co_t &co)
{
    sch->q.push(co);
}


inline void co_t::_wait(co_block_t &wq)
{
    wq.push(*this);
    sch->blocked_coroutine = this;
}

inline void co_t::_broadcast(co_block_t &wq)
{
    sch->q.append(wq);
}

inline void co_t::run()
{
    sch_t().run(*this);
}


//
// co_queue_t
//
inline bool co_queue_t::empty() const
{
    return first == nullptr;
}

inline co_t *co_queue_t::pop()
{
    co_t *node = first;
    if (first != nullptr) {
        first = first->next;
    }
    return node;
}

inline void co_queue_t::push(co_t &co)
{
    if (empty()) {
        first = last = &co;
    } else {
        last->next = &co;
        last = &co;
    }
    last->next = nullptr;
}

inline void co_queue_t::append(co_queue_t &q)
{
    if (!q.empty()) {
        if (empty()) {
            *this = q;
        } else {
            last->next = q.first;
            last = q.last;
        }
    }
}


//
// sch_t
//

inline void sch_t::run(co_t &entry)
{
    for (co_t *co = &entry; co != nullptr; co = q.pop()) {
        co->sch = this;
        blocked_coroutine = nullptr;
        co = co->step();
        if (co == nullptr) {
            // nop, remove co from concurrent queue
        } else if (blocked_coroutine != nullptr) {
            // nop, remove co from concurrent queue
        } else {
            q.push(*co);
        }
    }
}

#endif //COGO_CO_H
