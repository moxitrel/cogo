/** @file

* API
COGO_THIS
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END
CO_AWAIT      (COGO)
CO_ASYNC      (COGO)              : run a new coroutine concurrently.
CO_CHAN_WRITE (CHAN, MSG)       : send a message to channel
CO_CHAN_READ  (CHAN, MSG_NEXT)  : receive a message from channel, the result stored in cogo_msg_t.next

COGO_INIT       (COGO, OBJECT)
cogo_msg_t      : channel message type
cogo_chan_t         : channel type
COGO_CHAN_INIT  (N) : return a channel with capacity size_t
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

#ifndef COGO_T
    #define COGO_T cogo_async_t
#endif
typedef struct cogo_async cogo_async_t;

#ifndef COGO_SCHED_T
    #define COGO_SCHED_T cogo_async_sched_t
#endif
typedef struct cogo_async_sched cogo_async_sched_t;

#include "cogo_await.h"

#ifndef COGO_PRE_ASYNC
    #define COGO_PRE_ASYNC(COGO_THIS, COGO_OTHER)  // noop
#endif

#ifndef COGO_POST_ASYNC
    #define COGO_POST_ASYNC(COGO_THIS, COGO_OTHER)  // noop
#endif

#ifndef COGO_PRE_CHAN_READ
    #define COGO_PRE_CHAN_READ(COGO_THIS, CHAN, MSG)  // noop
#endif

#ifndef COGO_POST_CHAN_READ
    #define COGO_POST_CHAN_READ(COGO_THIS, CHAN, MSG)  // noop
#endif

#ifndef COGO_PRE_CHAN_WRITE
    #define COGO_PRE_CHAN_WRITE(COGO_THIS, CHAN, MSG)  // noop
#endif

#ifndef COGO_POST_CHAN_WRITE
    #define COGO_POST_CHAN_WRITE(COGO_THIS, CHAN, MSG)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

/// Implement concurrency.
/// @extends cogo_await_t
struct cogo_async {
    // inherit cogo_await_t (with scheduler)
    cogo_await_t await;

    // build coroutine list
    COGO_T* next;
};

#undef COGO_INIT
#define COGO_INIT(ASYNC, FUNC) COGO_ASYNC_INIT(ASYNC, FUNC)
#define COGO_ASYNC_INIT(COGO, FUNC)                 \
    {                                               \
            /*.await=*/COGO_AWAIT_INIT(COGO, FUNC), \
            /*.next=*/0,                            \
    }

#define COGO_ASYNC_OF(ASYNC) (ASYNC)
#undef COGO_AWAIT_OF
#define COGO_AWAIT_OF(COGO) (&COGO_ASYNC_OF(COGO)->await)
#define COGO_NEXT_OF(COGO)  (&COGO_ASYNC_OF(COGO)->next)

#undef COGO_IS_VALID
#define COGO_IS_VALID(ASYNC)      COGO_ASYNC_IS_VALID(ASYNC)
#define COGO_ASYNC_IS_VALID(COGO) COGO_AWAIT_IS_VALID(COGO)

#define COGO_QUEUE_VALUE_T        cogo_async_t
#define COGO_QUEUE_NEXT(ASYNC)    ((ASYNC)->next)
#include "private/cogo_queue_template.h"
#undef COGO_QUEUE_VALUE_T
#undef COGO_QUEUE_NEXT
#define COGO_CQ_T            COGO_QUEUE_T(cogo_async_t)
#define COGO_CQ_MAKE         COGO_QUEUE_MAKE(cogo_async_t)
#define COGO_CQ_IS_EMPTY     COGO_QUEUE_IS_EMPTY(cogo_async_t)
#define COGO_CQ_PUSH         COGO_QUEUE_PUSH(cogo_async_t)
#define COGO_CQ_POP          COGO_QUEUE_POP(cogo_async_t)
#define COGO_CQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(cogo_async_t)
struct cogo_async_sched {
    cogo_await_sched_t await_sched;

    // other coroutines running concurrently
    COGO_CQ_T cq;

    // global:
    //  struct cogo_async_sched* run; // running schedulers (idles not in list)
};

#undef COGO_SCHED_INIT
#define COGO_SCHED_INIT(ASYNC) COGO_ASYNC_SCHED_INIT(ASYNC)
#define COGO_ASYNC_SCHED_INIT(COGO)                       \
    {                                                     \
            /*.await_sched=*/COGO_AWAIT_SCHED_INIT(COGO), \
            /*.cq=*/COGO_CQ_MAKE(),                       \
    }

#define COGO_ASYNC_SCHED_OF(ASYNC_SCHED) (ASYNC_SCHED)
#undef COGO_AWAIT_SCHED_OF
#define COGO_AWAIT_SCHED_OF(SCHED) (&COGO_ASYNC_SCHED_OF(SCHED)->await_sched)
#define COGO_SCHED_CQ_OF(SCHED)    (&COGO_ASYNC_SCHED_OF(SCHED)->q)

// Add coroutine to the concurrent queue.
// Switch context if return non-zero.
int cogo_async_sched_add(cogo_async_sched_t* sched, cogo_async_t* cogo);

// return the next coroutine to be run, and remove it from the queue
COGO_T* cogo_async_sched_remove(cogo_async_sched_t* sched);

/// Start a new coroutine to run concurrently.
#define CO_ASYNC(COGO_OTHER) COGO_ASYNC(COGO_THIS, COGO_OTHER)
#define COGO_ASYNC(COGO, COGO_OTHER)                                                                     \
    do {                                                                                                 \
        COGO_ASSERT(COGO_IS_VALID(COGO) && COGO_IS_VALID(COGO_OTHER));                                   \
        COGO_PRE_ASYNC((+(COGO)), (+(COGO_OTHER)));                                                      \
        if (cogo_async_sched_add(COGO_ASYNC_SCHED_OF(COGO_SCHED_OF(COGO)), COGO_ASYNC_OF(COGO_OTHER))) { \
            COGO_DO_YIELD(COGO);                                                                         \
        }                                                                                                \
        COGO_POST_ASYNC((+(COGO)), (+(COGO_OTHER)));                                                     \
    } while (0)

// channel message
typedef struct cogo_msg {
    struct cogo_msg* next;
    union {
        char as_char;
        short as_short;
        int as_int;
        long as_long;
        float as_float;
        double as_double;
        void* as_pointer;
    } data;
} cogo_msg_t;

#define COGO_QUEUE_VALUE_T   cogo_msg_t
#define COGO_QUEUE_NEXT(MSG) ((MSG)->next)
#include "private/cogo_queue_template.h"
#undef COGO_QUEUE_VALUE_T
#undef COGO_QUEUE_NEXT
#define COGO_MQ_T            COGO_QUEUE_T(cogo_msg_t)
#define COGO_MQ_MAKE         COGO_QUEUE_MAKE(cogo_msg_t)
#define COGO_MQ_PUSH         COGO_QUEUE_PUSH(cogo_msg_t)
#define COGO_MQ_POP          COGO_QUEUE_POP(cogo_msg_t)
#define COGO_MQ_POP_NONEMPTY COGO_QUEUE_POP_NONEMPTY(cogo_msg_t)
typedef struct cogo_chan {
    // all coroutines blocked by this channel
    COGO_CQ_T cq;

    // message queue
    COGO_MQ_T mq;

    // current size
    // < 0  : some readers blocked
    // > capacity: some writers blocked
    long size;

    // max size
    long capacity;
} cogo_chan_t;

#define COGO_CHAN_INIT(N)           \
    {                               \
            /*.cq=*/COGO_CQ_MAKE(), \
            /*.mq=*/COGO_MQ_MAKE(), \
            /*.size=*/0,            \
            /*.capacity=*/(N),      \
    }

/** Receive a message from channel.
 If there's no message in channel, block until one arrived.

 @param[in] CHAN the channel where to receive message.
 @param[out] MSG_NEXT the address of received message is stored in MSG_NEXT->next.

 @pre This macro can only be called in the coroutine function, i.e. between CO_BEGIN and CO_END.
 @pre CHAN != NULL && MSG_NEXT != NULL
 @post At the time of read success (and before any other messages written by other coroutines), the size of channel is decreased by 1.
 @post MSG_NEXT->next != NULL

 @par Example
 @code
CO_DECLARE(coro_t, cogo_chan_t* chan, cogo_msg_t msg) {
  coro_t* thiz = (coro_t*)COGO_THIS;
  cogo_chan_t* chan = thiz->chan;
  cogo_msg_t* msg = &thiz->msg;
CO_BEGIN:

  CO_CHAN_READ(chan, msg);
  T* obj = (T*)msg->next;

CO_END:;
}
 @endcode
*/
#define CO_CHAN_READ(CHAN, MSG_NEXT) COGO_CHAN_READ(COGO_THIS, CHAN, MSG_NEXT)
#define COGO_CHAN_READ(COGO, CHAN, MSG_NEXT)                           \
    do {                                                               \
        COGO_ASSERT((CHAN) == (CHAN) && (MSG_NEXT) == (MSG_NEXT));     \
        COGO_PRE_CHAN_READ((+(COGO)), (+(CHAN)), (+(MSG_NEXT)));       \
        if (cogo_chan_read(COGO_ASYNC_OF(COGO), (CHAN), (MSG_NEXT))) { \
            COGO_DO_YIELD(COGO);                                       \
        }                                                              \
        COGO_POST_CHAN_READ((+(COGO)), (+(CHAN)), (+(MSG_NEXT)));      \
    } while (0)
// Switch context if return non-zero.
int cogo_chan_read(cogo_async_t* async, cogo_chan_t* chan, cogo_msg_t* msg_next);

/** Send a message through channel. If channel is full, block until available.
 @param[in] CHAN the channel where to send message.
 @param[out] MSG the message to send.
 @pre This macro can only be called in the coroutine function, i.e. between CO_BEGIN and CO_END, or COGO_BEGIN() and COGO_END()
 @pre CHAN != NULL && MSG != NULL
 @post At the time of writing success (and before any message has been read by other coroutines), the size of channel is increased by 1.
 @invariant the MSG body is not modified.
*/
#define CO_CHAN_WRITE(CHAN, MSG) COGO_CHAN_WRITE(COGO_THIS, CHAN, MSG)
#define COGO_CHAN_WRITE(COGO, CHAN, MSG)                           \
    do {                                                           \
        COGO_ASSERT((CHAN) == (CHAN) && (MSG) == (MSG));           \
        COGO_PRE_CHAN_WRITE((+(COGO)), (+(CHAN)), (+(MSG)));       \
        if (cogo_chan_write(COGO_ASYNC_OF(COGO), (CHAN), (MSG))) { \
            COGO_DO_YIELD(COGO);                                   \
        }                                                          \
        COGO_POST_CHAN_WRITE((+(COGO)), (+(CHAN)), (+(MSG)));      \
    } while (0)
// Switch context if return non-zero.
int cogo_chan_write(cogo_async_t* async, cogo_chan_t* chan, cogo_msg_t* msg);

#undef COGO_RESUME
#define COGO_RESUME(ASYNC) cogo_async_resume(ASYNC)
cogo_pc_t cogo_async_resume(cogo_async_t* async);

#define COGO_RUN(ASYNC) cogo_async_run(ASYNC)
void cogo_async_run(cogo_async_t* async);

#ifdef __cplusplus
}
#endif
#endif  // COGO_ASYNC_H_
