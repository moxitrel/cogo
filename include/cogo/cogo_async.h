/*

* API
co_this
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END
CO_AWAIT(CO)
CO_ASYNC(CO)    : run a new coroutine concurrently.
CO_CHAN_WRITE(CHAN, MSG)      : send a message to channel
CO_CHAN_READ (CHAN, MSG_NEXT) : receive a message from channel, the result stored in co_message_t.next

CO_INIT  (CO, TYPE, ...)
co_message_t    : channel message type
co_chan_t       : channel type
CO_CHAN_MAKE(N) : return a channel with capacity size_t
co_status_t
CO_STATUS(CO)
CO_RESUME(CO)
CO_RUN   (CO)

CO_DECLARE(TYPE, ...){}
CO_DEFINE (TYPE)     {}

cogo_async_t               : coroutine type
cogo_async_sched_t         : scheduler type

*/
#ifndef COGO_ASYNC_H_
#define COGO_ASYNC_H_

#include <stdbool.h>
#include <stddef.h>

#include "cogo_await.h"

#ifdef __cplusplus
extern "C" {
#endif

// implement concurrency
typedef struct cogo_async {
  // inherit cogo_await_t (with scheduler)
  cogo_await_t base_await;

  // build coroutine list
  struct cogo_async* next;
} cogo_async_t;

// COGO_QUEUE_T             (cogo_async_t)
// COGO_QUEUE_IS_EMPTY      (cogo_async_t) (const COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_POP           (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_POP_NONEMPTY  (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_PUSH          (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*, cogo_async_t*)
#undef COGO_QUEUE_ELEMENT_T
#undef COGO_QUEUE_NEXT
#define COGO_QUEUE_ELEMENT_T cogo_async_t
#define COGO_QUEUE_NEXT(CO)  ((CO)->next)
#include "_internal/cogo_queue_template.h"
#define COGO_CQ_T            COGO_QUEUE_T(cogo_async_t)
#define COGO_CQ_IS_EMPTY     COGO_QUEUE_IS_EMPTY(cogo_async_t)
#define COGO_CQ_PUSH         COGO_QUEUE_PUSH(cogo_async_t)
#define COGO_CQ_POP          COGO_QUEUE_POP(cogo_async_t)
#define COGO_CQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(cogo_async_t)
typedef struct cogo_async_sched {
  cogo_await_sched_t base_await;

  // other coroutines running concurrently
  COGO_CQ_T q;

  // global:
  //  struct cogo_async_sched* run; // running schedulers (idles not in list)
} cogo_async_sched_t;

// add coroutine to the concurrent queue
// switch context if return true
bool cogo_async_sched_push(cogo_async_sched_t* sched, cogo_async_t* co);

// return the next coroutine to be run, and remove it from the queue
cogo_async_t* cogo_async_sched_pop(cogo_async_sched_t* sched);

// CO_ASYNC(cogo_async_t*): start a new coroutine to run concurrently.
#define CO_ASYNC(CO)                                                                                                   \
  do {                                                                                                                 \
    if (cogo_async_sched_push((cogo_async_sched_t*)((cogo_async_t*)co_this)->base_await.sched, (cogo_async_t*)(CO))) { \
      CO_YIELD_BY_ASYNC;                                                                                               \
    }                                                                                                                  \
  } while (0)
#define CO_YIELD_BY_ASYNC CO_YIELD;

// channel message
typedef struct co_message {
  struct co_message* next;
} co_message_t;

// COGO_QUEUE_T             (co_message_t)
// COGO_QUEUE_IS_EMPTY      (co_message_t) (const COGO_QUEUE_T(co_message_t)*)
// COGO_QUEUE_POP           (co_message_t) (      COGO_QUEUE_T(co_message_t)*)
// COGO_QUEUE_POP_NONEMPTY  (co_message_t) (      COGO_QUEUE_T(co_message_t)*)
// COGO_QUEUE_PUSH          (co_message_t) (      COGO_QUEUE_T(co_message_t)*, co_message_t*)
#undef COGO_QUEUE_ELEMENT_T
#undef COGO_QUEUE_NEXT
#define COGO_QUEUE_ELEMENT_T co_message_t
#define COGO_QUEUE_NEXT(MSG) ((MSG)->next)
#include "_internal/cogo_queue_template.h"
#define COGO_MQ_T            COGO_QUEUE_T(co_message_t)
#define COGO_MQ_PUSH         COGO_QUEUE_PUSH(co_message_t)
#define COGO_MQ_POP          COGO_QUEUE_POP(co_message_t)
#define COGO_MQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(co_message_t)
typedef struct co_chan {
  // all coroutines blocked by this channel
  COGO_CQ_T cq;

  // message queue
  COGO_MQ_T mq;

  // current size
  // < 0  : some readers blocked
  // > cap: some writers blocked
  ptrdiff_t size;

  // max size
  ptrdiff_t cap;
} co_chan_t;

#define CO_CHAN_MAKE(N) ((co_chan_t){.cap = (N)})

// CO_CHAN_READ(co_chan_t*, co_message_t*);
// MSG_NEXT: the read message is MSG_NEXT->next
#define CO_CHAN_READ(CHAN, MSG_NEXT)                                                             \
  do {                                                                                           \
    if (cogo_chan_read((cogo_async_t*)co_this, (co_chan_t*)(CHAN), (co_message_t*)(MSG_NEXT))) { \
      CO_YIELD_BY_CHAN_READ;                                                                     \
    }                                                                                            \
  } while (0)
// switch context if return true
bool cogo_chan_read(cogo_async_t* co_this, co_chan_t* chan, co_message_t* msg_next);
#define CO_YIELD_BY_CHAN_READ CO_YIELD;

// CO_CHAN_WRITE(co_chan_t*, co_message_t*);
#define CO_CHAN_WRITE(CHAN, MSG)                                                             \
  do {                                                                                       \
    if (cogo_chan_write((cogo_async_t*)co_this, (co_chan_t*)(CHAN), (co_message_t*)(MSG))) { \
      CO_YIELD_BY_CHAN_WRITE;                                                                \
    }                                                                                        \
  } while (0)
// switch context if return true
bool cogo_chan_write(cogo_async_t* co_this, co_chan_t* chan, co_message_t* msg);
#define CO_YIELD_BY_CHAN_WRITE CO_YIELD;

#undef CO_DECLARE
#define CO_DECLARE(TYPE, ...) \
  COGO_DECLARE(TYPE, cogo_async_t base_async, __VA_ARGS__)

#undef CO_INIT
#define CO_INIT(CO, TYPE, ...) \
  ((TYPE){{.base_await = {.resume = TYPE##_resume, .top = (cogo_await_t*)(CO)}}, __VA_ARGS__})

#undef CO_RESUME
#define CO_RESUME(CO) cogo_async_resume((cogo_async_t*)(CO))
co_status_t cogo_async_resume(cogo_async_t* co);

#undef CO_RUN
#define CO_RUN(CO) cogo_async_run((cogo_async_t*)(CO))
void cogo_async_run(cogo_async_t* co);

#ifdef __cplusplus
}
#endif
#endif  // COGO_ASYNC_H_
