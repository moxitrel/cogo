/*

* API
co_this
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END
CO_AWAIT()
CO_ASYNC()      : run a new coroutine concurrently.
CO_CHAN_WRITE() : send a message to channel
CO_CHAN_READ()  : receive a message from channel, the result stored in co_message_t.next

CO_DECLARE(NAME, ...){...}
CO_DEFINE(NAME){...}
CO_MAKE(NAME, ...)
NAME_t
co_message_t    : channel message type
co_chan_t       : channel type
CO_CHAN_MAKE()  : return a channel with capacity size_t
CO_RESUME()
CO_RUN()

co_status()
cogo_async_t               : coroutine type
cogo_async_sched_t         : sheduler  type
cogo_async_sched_resume()  : like CO_RESUME()

*/
#ifndef COGO_ASYNC_H_
#define COGO_ASYNC_H_

#include <stddef.h>

#include "cogo_await.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_async cogo_async_t;
typedef struct cogo_async_sched cogo_async_sched_t;

// implement concurrency
struct cogo_async {
  // inherit cogo_await_t
  cogo_await_t super;

  // build coroutine list
  struct cogo_async* next;
};

// COGO_QUEUE_T             (cogo_async_t)
// COGO_QUEUE_IS_EMPTY      (cogo_async_t) (const COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_POP           (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_POP_NONEMPTY  (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_PUSH          (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*, cogo_async_t*)
#undef COGO_QUEUE_ELEMENT_T
#undef COGO_QUEUE_NEXT
#define COGO_QUEUE_ELEMENT_T cogo_async_t
#define COGO_QUEUE_NEXT(CO)  ((CO)->next)
#include "_private/cogo_queue_template.h"
#define COGO_CQ_T            COGO_QUEUE_T(cogo_async_t)
#define COGO_CQ_PUSH         COGO_QUEUE_PUSH(cogo_async_t)
#define COGO_CQ_POP          COGO_QUEUE_POP(cogo_async_t)
#define COGO_CQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(cogo_async_t)
struct cogo_async_sched {
  // inherent cogo_await_sched_t
  cogo_await_sched_t super;

  // other coroutines to run concurrently
  COGO_CQ_T q;

  // global:
  //  cogo_async_sched_t* run; // running schedulers (idles not in list)
};

// add coroutine to the concurrent queue
// switch context if return non-zero
int cogo_async_sched_push(cogo_async_sched_t* sched, cogo_async_t* co);

// return the next coroutine to be run, and remove it from the queue
cogo_async_t* cogo_async_sched_pop(cogo_async_sched_t* sched);

// CO_ASYNC(cogo_async_t*): start a new coroutine to run concurrently.
#define CO_ASYNC(CO)                                                                                             \
  do {                                                                                                           \
    if (cogo_async_sched_push((cogo_async_sched_t*)((cogo_await_t*)co_this)->sched, (cogo_async_t*)(CO)) != 0) { \
      CO_YIELD;                                                                                                  \
    }                                                                                                            \
  } while (0)

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
#include "_private/cogo_queue_template.h"
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
  ptrdiff_t size;

  // max size
  ptrdiff_t cap;
} co_chan_t;

#define CO_CHAN_MAKE(N) ((co_chan_t){.cap = (N)})

// CO_CHAN_READ(co_chan_t*, co_message_t*);
// MSG_NEXT: the read message is MSG_NEXT->next
#define CO_CHAN_READ(CHAN, MSG_NEXT)                                       \
  do {                                                                     \
    if (cogo_chan_read((cogo_async_t*)co_this, (CHAN), (MSG_NEXT)) != 0) { \
      CO_YIELD;                                                            \
    }                                                                      \
  } while (0)
int cogo_chan_read(cogo_async_t* co_this, co_chan_t* chan, co_message_t* msg_next);

// CO_CHAN_WRITE(co_chan_t*, co_message_t*);
#define CO_CHAN_WRITE(CHAN, MSG)                                                      \
  do {                                                                                \
    if (cogo_chan_write((cogo_async_t*)co_this, (CHAN), (co_message_t*)(MSG)) != 0) { \
      CO_YIELD;                                                                       \
    }                                                                                 \
  } while (0)
int cogo_chan_write(cogo_async_t* co_this, co_chan_t* chan, co_message_t* msg);

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_async_t super, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.super = {.func = NAME##_func}}, __VA_ARGS__})

cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched);

#ifdef COGO_USE_RESUME
// continue to run a suspended coroutine until yield or finished
#undef CO_RESUME
#define CO_RESUME(CO) cogo_async_resume((cogo_async_t*)(CO))
cogo_async_t* cogo_async_resume(cogo_async_t* co);
#endif

// run the coroutines until all finished
#undef CO_RUN
#define CO_RUN(CO) cogo_async_run((cogo_async_t*)(CO))
void cogo_async_run(cogo_async_t* co);

#ifdef __cplusplus
}
#endif
#endif  // COGO_ASYNC_H_
