#ifndef COGO_CO_H
#define COGO_CO_H

#include "await.hpp"

class co_t;
class co_queue_t;
class co_sch_t;

// co_t: support concurrency
//  .run  ()          : keep running until all coroutines finished
class co_t : public await_t {
    // build coroutine queue (run concurrently)
    co_t *next;

    // cache channel message
    // when write blocked, the stored value type is void *
    // when read  blocked, the stored value type is void **
    void *chan_msg;

    friend co_queue_t;
    friend co_sch_t;

protected:
    // only allow co_t (forbid await_t)
    void _await(co_t &co);  // hide await_t::_await()
    {
        await_t::_await(co);
    }

    void _start(co_t &);

public:
    // run until finish all
    void run();
};

// coroutine queue
class co_queue_t {
    co_t *head = nullptr;
    co_t *tail;

public:
    bool empty() const noexcept
    {
        return head == nullptr;
    }

    // dequeue, delete head node and return. If empty, return NULL.
    co_t *pop()
    {
        co_t *node = head;
        if (head != nullptr) {
            head = head->next;
        }
        return node;
    }

    // enqueue, append a coroutine to the end of queue.
    void push(co_t &co)
    {
        if (empty()) {
            head = &co;
        } else {
            tail->next = &co;
        }
        tail = &co;
        tail->next = nullptr;
    }

    // join another co_queue_t to the end of queue.
    void append(co_queue_t &q)
    {
        if (q.empty()) {
            return;
        }
        if (empty()) {
            *this = q;
            return;
        }
        tail->next = q.head;
        tail = q.tail;
    }
};

// co_t scheduler
class co_sch_t : public await_sch_t {
    // coroutines in this queue run concurrently
    co_queue_t q = {};
};


//
// co_t
//

// add a new coroutine to the scheduler.
inline void co_t::_start(co_t &co);
{
    co.sch = sch;
    ((co_sch_t *)sch)->q.push(co);
}

// co_start(co_t &)
#define co_start(CO)            \
do {                            \
    co_t::_start(CO);           \
    co_yield();                 \
} while (0)

// run until all finished.
// FIXME: scheduler stop successfully while expected blocking forever by channel.
inline void co_t::run()
{
    co_sch_t scheduler;

    scheduler.stack_top = this;
    sch = &scheduler;

    for (; scheduler.stack_top; scheduler.stack_top = (await_t *)co_queue_pop(&sch.q)) {
        scheduler.step();
        if (scheduler.stack_top) {
            scheduler.q.push((co_t *)scheduler.stack_top);
        }
    }
}


//
// channel
//


#endif //COGO_CO_H
