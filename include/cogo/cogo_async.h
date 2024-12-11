/** @file

* API
cogo_this
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END
CO_AWAIT      (COGO)
CO_ASYNC      (COGO)              : run a new coroutine concurrently.
CO_CHAN_WRITE (CHAN, MSG)       : send a message to channel
CO_CHAN_READ  (CHAN, MSG_NEXT)  : receive a message from channel, the result stored in co_message_t.next

COGO_INIT       (COGO, FUNC, ...)
co_message_t      : channel message type
co_chan_t         : channel type
CO_CHAN_MAKE  (N) : return a channel with capacity size_t
cogo_status_t
COGO_STATUS     (COGO)
COGO_RESUME     (COGO)
COGO_RUN        (COGO)

CO_DECLARE    (FUNC, ...){}
CO_DEFINE     (FUNC)     {}

cogo_async_t               : coroutine type
cogo_async_sched_t         : scheduler type

*/
#ifndef COGO_ASYNC_H_
#define COGO_ASYNC_H_

#ifndef COGO_ON_ASYNC
  #define COGO_ON_ASYNC(COGO)  // noop
#endif

#ifndef COGO_T
  #define COGO_T cogo_async_t
typedef struct cogo_async cogo_async_t;
#endif

#ifndef COGO_SCHED_T
  #define COGO_SCHED_T cogo_async_sched_t
typedef struct cogo_async_sched cogo_async_sched_t;
#endif

#include <stddef.h>

#include "cogo_await.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cogo_async cogo_async_t;
typedef struct cogo_async_sched cogo_async_sched_t;

// implement concurrency
struct cogo_async {
  // inherit cogo_await_t (with scheduler)
  cogo_await_t base_await;

  // build coroutine list
  COGO_T* next;
};

// COGO_QUEUE_T             (cogo_async_t)
// COGO_QUEUE_IS_EMPTY      (cogo_async_t) (const COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_POP           (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_POP_NONEMPTY  (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*)
// COGO_QUEUE_PUSH          (cogo_async_t) (      COGO_QUEUE_T(cogo_async_t)*, cogo_async_t*)
#undef COGO_QUEUE_ELEMENT_T
#undef COGO_QUEUE_NEXT
#define COGO_QUEUE_ELEMENT_T  cogo_async_t
#define COGO_QUEUE_NEXT(COGO) ((COGO)->next)
#include "private/cogo_queue_template.h"
#define COGO_CQ_T            COGO_QUEUE_T(cogo_async_t)
#define COGO_CQ_IS_EMPTY     COGO_QUEUE_IS_EMPTY(cogo_async_t)
#define COGO_CQ_PUSH         COGO_QUEUE_PUSH(cogo_async_t)
#define COGO_CQ_POP          COGO_QUEUE_POP(cogo_async_t)
#define COGO_CQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(cogo_async_t)
struct cogo_async_sched {
  cogo_await_sched_t base_await_sched;

  // other coroutines running concurrently
  COGO_CQ_T q;

  // global:
  //  struct cogo_async_sched* run; // running schedulers (idles not in list)
};

#define COGO_ASYNC_INIT(NAME, THIZ) \
  ((cogo_async_t){.base_await = COGO_AWAIT_INIT(NAME, THIZ)})

#undef COGO_AWAIT_V
#define COGO_ASYNC_V(ASYNC) (ASYNC)
#define COGO_AWAIT_V(ASYNC) (&COGO_ASYNC_V(ASYNC)->base_await)

// Add coroutine to the concurrent queue.
// Switch context if return non-zero.
int cogo_async_sched_push(cogo_async_sched_t* sched, cogo_async_t* cogo);

// return the next coroutine to be run, and remove it from the queue
COGO_T* cogo_async_sched_pop(cogo_async_sched_t* sched);

// CO_ASYNC(cogo_async_t*): start a new coroutine to run concurrently.
#define CO_ASYNC(DERIVANT1)                                                                     \
  do {                                                                                          \
    COGO_ON_ASYNC(+cogo_this);                                                                  \
    if (cogo_async_sched_push(cogo_this->base_await.sched, COGO_ASYNC_V(&(DERIVANT1)->cogo))) { \
      COGO_DO_YIELD(cogo_this);                                                                 \
    }                                                                                           \
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
#include "private/cogo_queue_template.h"
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

/** Receive a message from channel.
 If there's no message in channel, block until one arrived.

 @param[in] CHAN the channel where to receive message.
 @param[out] MSG_NEXT the address of received message is stored in MSG_NEXT->next.

 @pre This macro can only be called in the coroutine function, i.e. between CO_BEGIN and CO_END, or COGO_BEGIN() and COGO_END().
 @pre CHAN != NULL && MSG_NEXT != NULL
 @post At the time of read success (and before any other messages written by other coroutines), the size of channel is decreased by 1.
 @post MSG_NEXT->next != NULL

 @par Example
 @code
CO_DECLARE(coro, co_chan_t* chan, co_message_t msg) {
  coro_t* thiz = (coro_t*)cogo_this;
  co_chan_t* chan = thiz->chan;
  co_message_t* msg = &thiz->msg;
CO_BEGIN:

  CO_CHAN_READ(chan, msg);
  T* obj = (T*)msg->next;

CO_END:;
}
 @endcode
*/
#define CO_CHAN_READ(CHAN, MSG_NEXT)                                   \
  do {                                                                 \
    if (cogo_chan_read(COGO_ASYNC_V(cogo_this), (CHAN), (MSG_NEXT))) { \
      CO_YIELD;                                                        \
    }                                                                  \
  } while (0)
// Switch context if return non-zero.
int cogo_chan_read(cogo_async_t* cogo_this, co_chan_t* chan, co_message_t* msg_next);

/** Send a message through channel. If channel is full, block until available.
 @param[in] CHAN the channel where to send message.
 @param[out] MSG the message to send.
 @pre This macro can only be called in the coroutine function, i.e. between CO_BEGIN and CO_END, or COGO_BEGIN() and COGO_END()
 @pre CHAN != NULL && MSG != NULL
 @post At the time of writing success (and before any message has been read by other coroutines), the size of channel is increased by 1.
 @invariant the MSG body is not modified.
*/
#define CO_CHAN_WRITE(CHAN, MSG)                                   \
  do {                                                             \
    if (cogo_chan_write(COGO_ASYNC_V(cogo_this), (CHAN), (MSG))) { \
      CO_YIELD;                                                    \
    }                                                              \
  } while (0)
// Switch context if return non-zero.
int cogo_chan_write(cogo_async_t* cogo_this, co_chan_t* chan, co_message_t* msg);

#undef COGO_INIT
#define COGO_INIT COGO_ASYNC_INIT

#undef COGO_RESUME
#define COGO_RESUME(DERIVANT) cogo_async_resume(&(DERIVANT)->cogo)
cogo_pc_t cogo_async_resume(cogo_async_t* cogo_this);

#define COGO_RUN(DERIVANT) cogo_async_run(&(DERIVANT)->cogo)
void cogo_async_run(cogo_async_t* cogo_this);

#ifdef __cplusplus
}
#endif
#endif  // COGO_ASYNC_H_
