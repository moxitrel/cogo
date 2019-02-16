/*

* API
- co_start(co_t *, co_t *)  :: run a new coroutine concurrently.

- co_t CO(void(*)(co_t *))  :: co_t constructor.
- co_run(co_t *)            :: run until all coroutine finished.

*/
#ifndef COGO_CO_H
#define COGO_CO_H

#include "await.h"
#include <stddef.h>
#include <stdbool.h>

typedef struct co_t       co_t;
typedef struct co_queue_t co_queue_t;
typedef struct co_sch_t   co_sch_t;
typedef struct chan_t     chan_t;

// co_t: support concurrency
struct co_t {
    // inherit await_t
    await_t await_t;

    // build coroutine queue (run concurrently)
    co_t *next;
};
// co_t CO(void(*)(co_t *)): co_t constructor, **await_t.sch isn't inited**.
#define CO(FUN)         ((co_t){.await_t = AWAIT(FUN),})


//
// co_t queue
//
struct co_queue_t {
    co_t *head;
    co_t *tail;
};

inline static bool co_queue_empty(const co_queue_t *q)
{
    assert(q);
    return q->head == NULL;
}

// dequeue
inline static co_t *co_queue_pop(co_queue_t *q)
{
    assert(q);

    co_t *node = q->head;
    if (q->head != NULL) {
        q->head = q->head->next;
    }
    return node;
}

// enqueue
inline static void co_queue_push(co_queue_t *q, co_t *co)
{
    assert(q);
    assert(co);

    if (co_queue_empty(q)) {
        q->head = co;
    } else {
        q->tail->next = co;
    }
    q->tail = co;
    q->tail->next = NULL;
}

inline static void co_queue_merge(co_queue_t *q, const co_queue_t *q2)
{
    assert(q);
    assert(q2);

    if (co_queue_empty(q2)) {
        return;
    }
    if (co_queue_empty(q)) {
        *q = *q2;
        return;
    }
    q->tail->next = q2->head;
    q->tail = q2->tail;
}

// co_t scheduler
struct co_sch_t {
    // inherent await_sch_t
    await_sch_t await_sch_t;

    // all coroutines that run concurrently
    co_queue_t q;
};


// channel
struct chan_t {
    // all coroutines blocked by this channel
    co_queue_t rq;  // blocked when read
    co_queue_t wq;  // blocked when write

    bool read_blocked;  // for unbuffered channel, cap = 0

    // how many messages in buffer now
    unsigned int len;
    // the max number of messages can be buffered
    unsigned int cap;
    // TODO: implement message queue
    void *msg[1];   // size = cap + 1
};

#define CHAN()     ((chan_t){.cap = 0,})

static bool chan_try_read(co_t *co, chan_t *chan, void **msg_ptr)
{
    assert(co);
    assert(chan);
    assert(msg_ptr);

    bool ok;

    if (chan->len > 0) {
        // FIXME: pop message
        chan->len--;
        *msg_ptr = chan->msg[chan->len];

        ok = true;
    } else {
        // sleep in background
        co_queue_push(&chan->rq, co);       // put into blocking queue
        co->await_t.sch->stack_top = NULL;  // remove from scheduler

        // for unbuffered channel
        chan->read_blocked = true;

        ok = false;
    }

    // wake up a writer if exist
    co_t *writer = co_queue_pop(&chan->wq);
    if (writer) {
        co_queue_push(&((co_sch_t *)co->await_t.sch)->q, writer);
    }

    return ok;
}

static bool chan_try_write(co_t *co, chan_t *chan, void *msg)
{
    assert(co);
    assert(chan);

    bool ok;

    if (chan->cap + chan->read_blocked > chan->len) {
        // push message in buffer
        chan->msg[chan->len] = msg;
        chan->len++;

        // for unbuffered channel
        chan->read_blocked = false;

        ok = true;
    } else {
        // sleep in background
        co_queue_push(&chan->wq, co);
        co->await_t.sch->stack_top = NULL;

        ok = false;
    }

    // wake up a reader
    co_t *reader = co_queue_pop(&chan->rq);
    if (reader) {
        co_queue_push(&((co_sch_t *) co->await_t.sch)->q, reader);
    }

    return ok;
}

// co_chan_write(co_t *, chan_t *, void *);
// TODO: try to eliminate the side effect of args
#define co_chan_write(CO,CHAN,MSG)                                              \
do {                                                                            \
    while (!chan_try_write((co_t *)(CO), (CHAN), (MSG))) {                      \
        co_yield((co_t *)(CO));                                                 \
    }                                                                           \
} while (0)


// co_chan_read(co_t *, chan_t *, void **);
// TODO: try to eliminate the side effect of args
#define co_chan_read(CO,CHAN,MSG)                                               \
do {                                                                            \
    while (!chan_try_read((co_t *)(CO), (CHAN), (MSG))) {                       \
        co_yield((co_t *)(CO));                                                 \
    }                                                                           \
} while (0)


//
// co_t
//

inline static co_t *co__concur(co_t *co, co_t *co2)
{
    assert(co);
    assert(co2);
    
    co2->await_t.sch = co->await_t.sch;
    co_queue_push(&((co_sch_t *)co->await_t.sch)->q, co2);
    return co;
}
// co_start(co_t *, co_t *): add a new coroutine to the scheduler.
#define co_start(CO, CO2)  co_yield(co__concur((co_t *)(CO), (co_t *)(CO2)))


// run co as the entry coroutine, until all finished.
static void co_run(void *co)
{
    assert(co);

    // associate coroutine with a scheduler
    co_sch_t sch = {
        .await_sch_t = {.stack_top = (await_t *)co},
    };
    ((await_t *)co)->sch = &sch.await_sch_t;

    for (; sch.await_sch_t.stack_top; sch.await_sch_t.stack_top = (await_t *)co_queue_pop(&sch.q)) {
        sch_step(&sch.await_sch_t);
        if (sch.await_sch_t.stack_top) {
            co_queue_push(&sch.q, (co_t *)sch.await_sch_t.stack_top);
        }
    }
}

#endif //COGO_CO_H
