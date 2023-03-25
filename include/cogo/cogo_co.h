/*

* API
CO_BEGIN
CO_END
CO_YIELD
CO_RETURN
co_this
CO_DECLARE(NAME, ...){...}
CO_DEFINE(NAME){...}
CO_MAKE(NAME, ...)
NAME_t
CO_AWAIT(cogo_await_t*)
CO_SCHED_RESUME(cogo_co_t*)
CO_RUN(cogo_co_sched_t*)
co_msg_t                            : channel message type
co_chan_t                           : channel type
CO_START(cogo_co_t*)                : run a new coroutine concurrently.
CO_CHAN_MAKE(size_t)                : return a channel with capacity size_t
CO_CHAN_WRITE(co_chan_t*, co_msg_t*): send a message to channel
CO_CHAN_READ(co_chan_t*, co_msg_t*) : receive a message from channel, the result stored in co_msg_t.next

co_status(cogo_yield_t*)
NAME_func
cogo_await_t
cogo_await_sched_t
cogo_co_t                           : coroutine type
cogo_co_sched_t                     : sheduler  type
*/
#ifndef COGO_COGO_CO_H_
#define COGO_COGO_CO_H_

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
#include "cogo_queue_template_st.h"
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

// continue to run a suspended coroutine until yield or finished
cogo_co_t* cogo_co_sched_step(cogo_co_sched_t* sched);

// CO_START(cogo_co_t*): add a new coroutine to the scheduler.
#define CO_START(CO)                                                                                    \
  do {                                                                                                  \
    if (cogo_co_sched_push((cogo_co_sched_t*)((cogo_await_t*)co_this)->sched, (cogo_co_t*)(CO)) != 0) { \
      CO_YIELD;                                                                                         \
    }                                                                                                   \
  } while (0)

// channel message
typedef struct co_msg {
  struct co_msg* next;
} co_msg_t;

// COGO_QUEUE_T             (co_msg_t)
// COGO_QUEUE_IS_EMPTY      (co_msg_t) (const COGO_QUEUE_T(co_msg_t)*)
// COGO_QUEUE_POP           (co_msg_t) (      COGO_QUEUE_T(co_msg_t)*)
// COGO_QUEUE_POP_NONEMPTY  (co_msg_t) (      COGO_QUEUE_T(co_msg_t)*)
// COGO_QUEUE_PUSH          (co_msg_t) (      COGO_QUEUE_T(co_msg_t)*, co_msg_t*)
#undef COGO_QUEUE_ELEMENT_T
#undef COGO_QUEUE_NEXT
#define COGO_QUEUE_ELEMENT_T co_msg_t
#define COGO_QUEUE_NEXT(MSG) ((MSG)->next)
#include "cogo_queue_template_st.h"
#define COGO_MQ_T            COGO_QUEUE_T(co_msg_t)
#define COGO_MQ_PUSH         COGO_QUEUE_PUSH(co_msg_t)
#define COGO_MQ_POP          COGO_QUEUE_POP(co_msg_t)
#define COGO_MQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(co_msg_t)
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

// CO_CHAN_READ(co_chan_t*, co_msg_t*);
// MSG_NEXT: the read message is MSG_NEXT->next
#define CO_CHAN_READ(CHAN, MSG_NEXT)                                    \
  do {                                                                  \
    if (cogo_chan_read((cogo_co_t*)co_this, (CHAN), (MSG_NEXT)) != 0) { \
      CO_YIELD;                                                         \
    }                                                                   \
  } while (0)
int cogo_chan_read(cogo_co_t* co_this, co_chan_t* chan, co_msg_t* msg_next);

// CO_CHAN_WRITE(co_chan_t*, co_msg_t*);
#define CO_CHAN_WRITE(CHAN, MSG)                                               \
  do {                                                                         \
    if (cogo_chan_write((cogo_co_t*)co_this, (CHAN), (co_msg_t*)(MSG)) != 0) { \
      CO_YIELD;                                                                \
    }                                                                          \
  } while (0)
int cogo_chan_write(cogo_co_t* co_this, co_chan_t* chan, co_msg_t* msg);

// continue to run a suspended coroutine until yield or finished
#undef CO_SCHED_RESUME
#define CO_SCHED_RESUME(CO) cogo_co_resume((cogo_co_t*)(CO))
cogo_co_t* cogo_co_resume(cogo_co_t* co);

// run the coroutines until all finished
#undef CO_RUN
#define CO_RUN(MAIN) cogo_co_run((cogo_co_t*)(MAIN))
void cogo_co_run(cogo_co_t* co_main);

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, cogo_co_t super, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.super = {.func = NAME##_func}}, __VA_ARGS__})

#ifdef __cplusplus
}
#endif
#endif  // COGO_COGO_CO_H_
