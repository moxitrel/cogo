/*

* API
co_this
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END
CO_DECLARE(NAME, ...){...}
CO_DEFINE(NAME){...}
CO_MAKE(NAME, ...)
NAME_t
CO_AWAIT()
CO_RESUME()
CO_RUN()
CO_START()          : run a new coroutine concurrently.
co_message_t        : channel message type
co_channel_t        : channel type
CO_CHANNEL_MAKE()   : return a channel with capacity size_t
CO_CHANNEL_WRITE()  : send a message to channel
CO_CHANNEL_READ()   : receive a message from channel, the result stored in co_message_t.next

co_status()
cogo_co_t               : coroutine type
cogo_co_sched_t         : sheduler  type
cogo_co_sched_resume()  : like CO_RESUME()

*/
#ifndef COGO_CO_H_
#define COGO_CO_H_

#include <stddef.h>

#include "cogo_await.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_co {
  // inherit cogo_await_t
  cogo_await_t super;

  // build coroutine list
  struct cogo_co* next;
} cogo_co_t;

// COGO_QUEUE_T             (cogo_co_t)
// COGO_QUEUE_IS_EMPTY      (cogo_co_t) (const COGO_QUEUE_T(cogo_co_t)*)
// COGO_QUEUE_POP           (cogo_co_t) (      COGO_QUEUE_T(cogo_co_t)*)
// COGO_QUEUE_POP_NONEMPTY  (cogo_co_t) (      COGO_QUEUE_T(cogo_co_t)*)
// COGO_QUEUE_PUSH          (cogo_co_t) (      COGO_QUEUE_T(cogo_co_t)*, cogo_co_t*)
#undef COGO_QUEUE_ELEMENT_T
#undef COGO_QUEUE_NEXT
#define COGO_QUEUE_ELEMENT_T cogo_co_t
#define COGO_QUEUE_NEXT(CO)  ((CO)->next)
#include "cogo_queue_template.h"
#define COGO_CQ_T            COGO_QUEUE_T(cogo_co_t)
#define COGO_CQ_PUSH         COGO_QUEUE_PUSH(cogo_co_t)
#define COGO_CQ_POP          COGO_QUEUE_POP(cogo_co_t)
#define COGO_CQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(cogo_co_t)
typedef struct cogo_co_sched {
  // inherent cogo_await_sched_t
  cogo_await_sched_t super;

  // other coroutines to run concurrently
  COGO_CQ_T q;
} cogo_co_sched_t;

// add coroutine to the concurrent queue
// switch context if return non-zero
int cogo_co_sched_push(cogo_co_sched_t* sched, cogo_co_t* co);

// return the next coroutine to be run, and remove it from the queue
cogo_co_t* cogo_co_sched_pop(cogo_co_sched_t* sched);

// CO_START(cogo_co_t*): add a new coroutine to the scheduler.
#define CO_START(CO)                                                                                    \
  do {                                                                                                  \
    if (cogo_co_sched_push((cogo_co_sched_t*)((cogo_await_t*)co_this)->sched, (cogo_co_t*)(CO)) != 0) { \
      CO_YIELD;                                                                                         \
    }                                                                                                   \
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
#include "cogo_queue_template.h"
#define COGO_MQ_T            COGO_QUEUE_T(co_message_t)
#define COGO_MQ_PUSH         COGO_QUEUE_PUSH(co_message_t)
#define COGO_MQ_POP          COGO_QUEUE_POP(co_message_t)
#define COGO_MQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(co_message_t)
typedef struct co_channel {
  // all coroutines blocked by this channel
  COGO_CQ_T cq;

  // message queue
  COGO_MQ_T mq;

  // current size
  ptrdiff_t size;

  // max size
  ptrdiff_t cap;
} co_channel_t;

#define CO_CHANNEL_MAKE(N) ((co_channel_t){.cap = (N)})

// CO_CHANNEL_READ(co_channel_t*, co_message_t*);
// MSG_NEXT: the read message is MSG_NEXT->next
#define CO_CHANNEL_READ(CHAN, MSG_NEXT)                                 \
  do {                                                                  \
    if (cogo_chan_read((cogo_co_t*)co_this, (CHAN), (MSG_NEXT)) != 0) { \
      CO_YIELD;                                                         \
    }                                                                   \
  } while (0)
int cogo_chan_read(cogo_co_t* co_this, co_channel_t* chan, co_message_t* msg_next);

// CO_CHANNEL_WRITE(co_channel_t*, co_message_t*);
#define CO_CHANNEL_WRITE(CHAN, MSG)                                                \
  do {                                                                             \
    if (cogo_chan_write((cogo_co_t*)co_this, (CHAN), (co_message_t*)(MSG)) != 0) { \
      CO_YIELD;                                                                    \
    }                                                                              \
  } while (0)
int cogo_chan_write(cogo_co_t* co_this, co_channel_t* chan, co_message_t* msg);

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_co_t super, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.super = {.func = NAME##_func}}, __VA_ARGS__})

cogo_co_t* cogo_co_sched_resume(cogo_co_sched_t* const sched);

// continue to run a suspended coroutine until yield or finished
#undef CO_RESUME
#define CO_RESUME(CO) cogo_co_resume((cogo_co_t*)(CO))
cogo_co_t* cogo_co_resume(cogo_co_t* co);

// run the coroutines until all finished
#undef CO_RUN
#define CO_RUN(MAIN) cogo_co_run((cogo_co_t*)(MAIN))
void cogo_co_run(cogo_co_t* co_main);

#ifdef __cplusplus
}
#endif
#endif  // COGO_CO_H_
