/*

* API
- co_sched(co_t *, co_t *)  :: run a new coroutine concurrently.

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
typedef struct chan_t     chan_t;
typedef struct sch_co_t   sch_co_t;

// co_t: support concurrency
struct co_t {
    // inherit await_t
    await_t await;

    // build coroutine queue (run concurrently)
    co_t *next;
};
// co_t CO(void(*)(co_t *)): co_t constructor, await.sch isn't inited.
#define CO(FUN)         ((co_t){.await = AWAIT(FUN),})

// co_t queue
struct co_queue_t {
    co_t *head;
    co_t *tail;
};
inline static bool  co_queue_empty (const co_queue_t *);
inline static co_t *co_queue_pop   (co_queue_t *);          // dequeue
inline static void  co_queue_push  (co_queue_t *, co_t *);  // enqueue
inline static void  co_queue_append(co_queue_t *, const co_queue_t *);


// channel (like golang)
struct chan_t {
    // all the coroutines blocked by this channel
    co_queue_t rq;  // read  blocked
    co_queue_t wq;  // write blocked

    // how many messages sent now
    unsigned int len;

    // the max number of messages can be buffered
    unsigned int cap;

    // message queue
    // TODO: support message buffer
    void *msg[1];
};
#define CHAN(N)     ((chan_t){.cap = 0,})
inline static unsigned int chan_len(chan_t *);
inline static unsigned int chan_cap(chan_t *);


// co_t scheduler
struct sch_co_t {
    // inherent await_sch_t
    await_sch_t sch_await;

    // all coroutines that run concurrently
    co_queue_t q;
};
#define SCH_CO(CO)  ((sch_co_t){.sch_await = AWAIT_SCH(CO),})

//
// co_t
//

// co_sched(co_t *, co_t *): add a new coroutine to the scheduler.
#define co_sched(SELF, CO)                          \
do {                                                \
    co_t *_self = (co_t *)(SELF);                   \
    co_t *_co   = (co_t *)(CO);                     \
                                                    \
    _co->await.sch = _self->await.sch;              \
    co_queue_push(&((sch_co_t *)_self->await.sch)->q, _co); \
    co_yield(_self);                                \
} while (0)

// co_run(co_t *): run CO as the entry, until all coroutines finished.
#define co_run(CO)  sch_co_run((co_t *)(CO))


//
// co_queue_t
//
bool co_queue_empty(const co_queue_t *self)
{
    assert(self);
    return self->head == NULL;
}

co_t *co_queue_pop(co_queue_t *self)
{
    assert(self);
    
    co_t *node = self->head;
    if (!co_queue_empty(self)) {
        self->head = self->head->next;
    }
    return node;
}

void co_queue_push(co_queue_t *self, co_t *co)
{
    assert(self);
    assert(co);
    
    if (co_queue_empty(self)) {
        self->head = self->tail = co;
    } else {
        self->tail->next = co;
        self->tail = co;
    }
    self->tail->next = NULL;
}

void co_queue_append(co_queue_t *self, const co_queue_t *q)
{
    assert(self);
    assert(q);

    if (co_queue_empty(q)) {
        return;
    }
    if (co_queue_empty(self)) {
        *self = *q;
        return;
    }
    self->tail->next = q->head;
    self->tail = q->tail;
}


//
// chan_t
//
unsigned int chan_len(chan_t *self)
{
    return self->len;
}

unsigned int chan_cap(chan_t *self)
{
    return self->cap;
}

static co_t *co_chan_try_read(co_t *co, chan_t *chan, void **msg_ptr)
{
    assert(co);
    assert(chan);
    assert(msg_ptr);

    if (chan_len(chan) < 1) {               // channel is empty
        co_queue_push(&chan->rq, co);       // record in channel
        co->await.sch->stack_top = NULL;    // remove from scheduler
        return co;
    } else {
        chan->len--;
        *msg_ptr = chan->msg[chan->len];

        co_t *w = co_queue_pop(&chan->wq);
        if (w) {
            co_queue_push(&((sch_co_t *)co->await.sch)->q, w);
        }

        return NULL;
    }
}

static co_t *co_chan_try_write(co_t *co, chan_t *chan, void *msg)
{
    assert(co);
    assert(chan);

    if (chan_len(chan) > chan_cap(chan)) {  // channel is empty
        co_queue_push(&chan->wq, co);       // record in channel
        co->await.sch->stack_top = NULL;    // remove from scheduler
        return co;
    } else {
        chan->msg[chan->len] = msg;
        chan->len++;

        co_t *r = co_queue_pop(&chan->rq);
        if (r) {
            co_queue_push(&((sch_co_t *)co->await.sch)->q, r);
        }
        return NULL;
    }
}

// co_chan_write(co_t *, chan_t *, void *);
// TODO: eliminate side effect of args
#define co_chan_write(SELF,CHAN,MSG)                                            \
do {                                                                            \
    for (;;) {                                                                  \
        co_t *_self = (co_t *)(SELF);                                           \
        co_t *ret = co_chan_try_write(_self,(CHAN),(MSG));                      \
        co_yield(_self);                                                        \
        if (!ret) {                                                             \
            break;                                                              \
        }                                                                       \
    }                                                                           \
} while (0)

// co_chan_read(co_t *, chan_t *, void **);
// TODO: eliminate side effect of args
#define co_chan_read(SELF,CHAN,MSG)                                             \
do {                                                                            \
    co_t *_self;                                                                \
    for (;(_self = co_chan_try_read((co_t *)(SELF),(CHAN),(MSG))) != NULL;) {   \
        co_yield(_self);                                                        \
    }                                                                           \
} while (0)


//
// sch_co_t
//

// run co as the entry coroutine, until all finished.
static void sch_co_run(co_t *co)
{
    assert(co);

    sch_co_t sch = SCH_CO(co);
    for (co->await.sch = &sch.sch_await; co; co = co_queue_pop(&sch.q)) {
        sch.sch_await.stack_top = &co->await;
        await_sch_step(&sch.sch_await);
        if (sch.sch_await.stack_top) {
            co_queue_push(&sch.q, (co_t *)sch.sch_await.stack_top);
        }
    }
}

#endif //COGO_CO_H
