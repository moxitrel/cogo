/** @file

* API
COGO_THIS
CO_BEGIN
CO_YIELD
CO_RETURN
CO_END
CO_AWAIT      (COGO)
CO_ASYNC      (COGO)            : run a new coroutine concurrently.
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

#include "cogo_call.h"

/// Add a coroutine to the running queue.
/// @return Switch context if return non-zero.
#ifndef COGO_SCHED_ADD
    #define COGO_SCHED_ADD(SCHED, COGO) cogo_async_sched_add(SCHED, COGO)
int cogo_async_sched_add(cogo_async_sched_t* sched, cogo_async_t* cogo);
#endif

// Pop the next coroutine to be run.
#ifndef COGO_SCHED_REMOVE
    #define COGO_SCHED_REMOVE(SCHED) cogo_async_sched_remove(SCHED)
COGO_T* cogo_async_sched_remove(cogo_async_sched_t* sched);
#endif

#ifndef COGO_BEFORE_ASYNC
    #define COGO_BEFORE_ASYNC(COGO, COGO_ASYNCEE)  // noop
#endif

#ifndef COGO_AFTER_ASYNC
    #define COGO_AFTER_ASYNC(COGO, COGO_ASYNCEE)  // noop
#endif

#ifndef COGO_BEFORE_CHAN_READ
    #define COGO_BEFORE_CHAN_READ(COGO, CHAN, MSG)  // noop
#endif

#ifndef COGO_AFTER_CHAN_READ
    #define COGO_AFTER_CHAN_READ(COGO, CHAN, MSG)  // noop
#endif

#ifndef COGO_BEFORE_CHAN_WRITE
    #define COGO_BEFORE_CHAN_WRITE(COGO, CHAN, MSG)  // noop
#endif

#ifndef COGO_AFTER_CHAN_WRITE
    #define COGO_AFTER_CHAN_WRITE(COGO, CHAN, MSG)  // noop
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct cogo_async {
    // inherit cogo_call_t (with scheduler)
    cogo_call_t call;

    // build coroutine list
    COGO_T* next;
};

#define COGO_ASYNC_INIT(COGO, FUNC)               \
    {                                             \
            /*.call=*/COGO_CALL_INIT(COGO, FUNC), \
            /*.next=*/0,                          \
    }
#define COGO_ASYNC_CALL(ASYNC)     (&(ASYNC)->call)
#define COGO_ASYNC_NEXT(ASYNC)     ((ASYNC)->next)
#define COGO_ASYNC_IS_VALID(ASYNC) COGO_CALL_IS_VALID(COGO_ASYNC_CALL(ASYNC))

#undef COGO_INIT
#undef COGO_CALL_OF
#undef COGO_IS_VALID
#define COGO_INIT(COGO, FUNC)  COGO_ASYNC_INIT(COGO, FUNC)
#define COGO_ASYNC_OF(COGO)    (COGO)
#define COGO_CALL_OF(COGO)     COGO_ASYNC_CALL(COGO_ASYNC_OF(COGO))
#define COGO_NEXT_OF(COGO)     COGO_ASYNC_NEXT(COGO_ASYNC_OF(COGO))
#define COGO_IS_VALID(COGO)    ((COGO) == (COGO) && (COGO) && COGO_ASYNC_IS_VALID(COGO_ASYNC_OF(COGO)))

#define COGO_QUEUE_VALUE_T     cogo_async_t
#define COGO_QUEUE_NEXT(ASYNC) ((ASYNC)->next)
#include "private/cogo_queue_template.h"
#undef COGO_QUEUE_VALUE_T
#undef COGO_QUEUE_NEXT
#define COGO_CQ_T               COGO_QUEUE_T(cogo_async_t)
#define COGO_CQ_MAKE            COGO_QUEUE_MAKE(cogo_async_t)
#define COGO_CQ_IS_EMPTY        COGO_QUEUE_IS_EMPTY(cogo_async_t)
#define COGO_CQ_ADD             COGO_QUEUE_ADD(cogo_async_t)
#define COGO_CQ_REMOVE          COGO_QUEUE_REMOVE(cogo_async_t)
#define COGO_CQ_REMOVE_NONEMPTY COGO_QUEUE_REMOVE_NONEMPTY(cogo_async_t)
struct cogo_async_sched {
    cogo_call_sched_t callsched;

    // other coroutines running concurrently
    COGO_CQ_T cq;

    // global:
    //  struct cogo_async_sched* run; // running schedulers (idles not in list)
};

#define COGO_ASYNCSCHED_INIT(COGO)                     \
    {                                                  \
            /*.call_sched=*/COGO_CALLSCHED_INIT(COGO), \
            /*.cq=*/COGO_CQ_MAKE(),                    \
    }
#define COGO_ASYNCSCHED_CALLSCHED(ASYNCSCHED) (&(ASYNCSCHED)->callsched)
#define COGO_ASYNCSCHED_CQ(ASYNCSCHED)        (&(ASYNCSCHED)->cq)
#define COGO_ASYNCSCHED_IS_VALID(ASYNCSCHED)  COGO_CALLSCHED_IS_VALID(COGO_ASYNCSCHED_CALLSCHED(ASYNCSCHED))

#undef COGO_SCHED_INIT
#undef COGO_CALLSCHED_OF
#define COGO_SCHED_INIT(COGO)     COGO_ASYNCSCHED_INIT(COGO)
#define COGO_ASYNCSCHED_OF(SCHED) (SCHED)
#define COGO_CALLSCHED_OF(SCHED)  COGO_ASYNCSCHED_CALLSCHED(COGO_ASYNCSCHED_OF(SCHED))
#define COGO_SCHED_CQ_OF(SCHED)   COGO_ASYNCSCHED_CQ(COGO_ASYNCSCHED_OF(SCHED))

#undef COGO_SCHED_IS_VALID
#define COGO_SCHED_IS_VALID(SCHED) ((SCHED) == (SCHED) && (SCHED) && COGO_ASYNCSCHED_IS_VALID(COGO_ASYNCSCHED_OF(SCHED)))

/// Start a new coroutine to run concurrently.
#define COGO_ASYNC(COGO, COGO_ASYNCEE)                                   \
    do {                                                                 \
        COGO_ASSERT(COGO_IS_VALID(COGO) && COGO_IS_VALID(COGO_ASYNCEE)); \
        COGO_BEFORE_ASYNC((+(COGO)), (+(COGO_ASYNCEE)));                 \
        if (COGO_SCHED_ADD(COGO_SCHED_OF(COGO), COGO_ASYNCEE)) {         \
            COGO_DO_YIELD(COGO);                                         \
        }                                                                \
        COGO_AFTER_ASYNC((+(COGO)), (+(COGO_ASYNCEE)));                  \
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
        void* as_void_p;
    } data;
} cogo_msg_t;

#define COGO_QUEUE_VALUE_T   cogo_msg_t
#define COGO_QUEUE_NEXT(MSG) ((MSG)->next)
#include "private/cogo_queue_template.h"
#undef COGO_QUEUE_VALUE_T
#undef COGO_QUEUE_NEXT
#define COGO_MQ_T               COGO_QUEUE_T(cogo_msg_t)
#define COGO_MQ_MAKE            COGO_QUEUE_MAKE(cogo_msg_t)
#define COGO_MQ_IS_EMPTY        COGO_QUEUE_IS_EMPTY(cogo_msg_t)
#define COGO_MQ_ADD             COGO_QUEUE_ADD(cogo_msg_t)
#define COGO_MQ_REMOVE          COGO_QUEUE_REMOVE(cogo_msg_t)
#define COGO_MQ_REMOVE_NONEMPTY COGO_QUEUE_REMOVE_NONEMPTY(cogo_msg_t)
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

#define COGO_CHAN_IS_VALID(CHAN) ((CHAN) == (CHAN) && (CHAN) && (CHAN)->capacity >= 0 && (CHAN)->size > PTRDIFF_MIN)

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
#define COGO_CHAN_READ(COGO, CHAN, MSG_NEXT)                                                                    \
    do {                                                                                                        \
        COGO_ASSERT(COGO_IS_VALID(COGO) && COGO_CHAN_IS_VALID(CHAN) && (MSG_NEXT) == (MSG_NEXT) && (MSG_NEXT)); \
        COGO_BEFORE_CHAN_READ((+(COGO)), (+(CHAN)), (+(MSG_NEXT)));                                             \
        if (cogo_chan_read(COGO_ASYNC_OF(COGO), (CHAN), (MSG_NEXT))) {                                          \
            COGO_DO_YIELD(COGO);                                                                                \
        }                                                                                                       \
        COGO_AFTER_CHAN_READ((+(COGO)), (+(CHAN)), (+(MSG_NEXT)));                                              \
    } while (0)
// Switch context if return non-zero.
int cogo_chan_read(cogo_async_t* cogo, cogo_chan_t* chan, cogo_msg_t* msg_next);

/** Send a message through channel. If channel is full, block until available.
 @param[in] CHAN the channel where to send message.
 @param[out] MSG the message to send.
 @pre This macro can only be called in the coroutine function, i.e. between CO_BEGIN and CO_END, or COGO_BEGIN() and COGO_END()
 @pre CHAN != NULL && MSG != NULL
 @post At the time of writing success (and before any message has been read by other coroutines), the size of channel is increased by 1.
 @invariant the MSG body is not modified.
*/
#define COGO_CHAN_WRITE(COGO, CHAN, MSG)                                                         \
    do {                                                                                         \
        COGO_ASSERT(COGO_IS_VALID(COGO) && COGO_CHAN_IS_VALID(CHAN) && (MSG) == (MSG) && (MSG)); \
        COGO_BEFORE_CHAN_WRITE((+(COGO)), (+(CHAN)), (+(MSG)));                                  \
        if (cogo_chan_write(COGO_ASYNC_OF(COGO), (CHAN), (MSG))) {                               \
            COGO_DO_YIELD(COGO);                                                                 \
        }                                                                                        \
        COGO_AFTER_CHAN_WRITE((+(COGO)), (+(CHAN)), (+(MSG)));                                   \
    } while (0)
// Switch context if return non-zero.
int cogo_chan_write(cogo_async_t* cogo, cogo_chan_t* chan, cogo_msg_t* msg);

#undef COGO_SCHED_RESUME
#define COGO_SCHED_RESUME(SCHED) cogo_async_sched_resume(SCHED)
cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* sched);

#undef COGO_RESUME
#define COGO_RESUME(COGO) cogo_async_resume(COGO)
const cogo_async_t* cogo_async_resume(cogo_async_t* cogo);

#define COGO_RUN(COGO) cogo_async_run(COGO)
void cogo_async_run(cogo_async_t* cogo);

#define CO_ASYNC(COGO_ASYNCEE)       COGO_ASYNC(COGO_THIS, COGO_ASYNCEE)
#define CO_CHAN_READ(CHAN, MSG_NEXT) COGO_CHAN_READ(COGO_THIS, CHAN, MSG_NEXT)
#define CO_CHAN_WRITE(CHAN, MSG)     COGO_CHAN_WRITE(COGO_THIS, CHAN, MSG)

#ifdef __cplusplus
}
#endif
#endif  // COGO_ASYNC_H_
