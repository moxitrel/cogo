// Copyright (c) 2018-2025 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

/// @file
#ifndef COGO_PARAL_H_
#define COGO_PARAL_H_

#ifndef COGO_T
    #define COGO_T COGO_PARAL_T
#endif
#define COGO_PARAL_T cogo_paral_t
typedef struct cogo_paral cogo_paral_t;

#ifndef COGO_SCHED_T
    #define COGO_SCHED_T COGO_PARALSCHED_T
#endif
#define COGO_PARALSCHED_T cogo_paralsched_t
typedef struct cogo_paralsched cogo_paralsched_t;

/// Add a coroutine to the running queue.
/// @return Switch context if return non-zero.
#ifndef COGO_SCHED_ADD
    #define COGO_SCHED_ADD(SCHED, COGO) COGO_PARALSCHED_ADD(SCHED, COGO)
#endif

/// Pop the next coroutine to be run.
#ifndef COGO_SCHED_REMOVE
    #define COGO_SCHED_REMOVE(SCHED) COGO_PARALSCHED_REMOVE(SCHED)
#endif

#include "cogo_call.h"

#ifndef COGO_BEFORE_ASYNC
    #define COGO_BEFORE_ASYNC(COGO, COGO_OTHER) COGO_NOOP(COGO, COGO_OTHER)
#endif

#ifndef COGO_AFTER_ASYNC
    #define COGO_AFTER_ASYNC(COGO, COGO_OTHER) COGO_NOOP(COGO, COGO_OTHER)
#endif

#ifndef COGO_BEFORE_CHAN_READ
    #define COGO_BEFORE_CHAN_READ(COGO, CHAN, MSG) COGO_NOOP(COGO, CHAN, MSG)
#endif

#ifndef COGO_AFTER_CHAN_READ
    #define COGO_AFTER_CHAN_READ(COGO, CHAN, MSG) COGO_NOOP(COGO, CHAN, MSG)
#endif

#ifndef COGO_BEFORE_CHAN_WRITE
    #define COGO_BEFORE_CHAN_WRITE(COGO, CHAN, MSG) COGO_NOOP(COGO, CHAN, MSG)
#endif

#ifndef COGO_AFTER_CHAN_WRITE
    #define COGO_AFTER_CHAN_WRITE(COGO, CHAN, MSG) COGO_NOOP(COGO, CHAN, MSG)
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct cogo_paral {
    // inherit cogo_call_t (with scheduler)
    COGO_CALL_T call;

    // build coroutine list
    COGO_T* next;
};

const COGO_PARAL_T* cogo_paral_resume(COGO_PARAL_T* paral);
void cogo_paral_run(COGO_PARAL_T* paral);

#define COGO_QUEUE_VALUE_T     COGO_T
#define COGO_QUEUE_NEXT(PARAL) ((PARAL)->next)
#include "private/cogo_queue_template.h"
#undef COGO_QUEUE_VALUE_T
#undef COGO_QUEUE_NEXT
#define COGO_CQ_T               COGO_QUEUE_T(COGO_T)
#define COGO_CQ_MAKE            COGO_QUEUE_MAKE(COGO_T)
#define COGO_CQ_ADD             COGO_QUEUE_ADD(COGO_T)
#define COGO_CQ_REMOVE          COGO_QUEUE_REMOVE(COGO_T)
#define COGO_CQ_REMOVE_NONEMPTY COGO_QUEUE_REMOVE_NONEMPTY(COGO_T)
struct cogo_paralsched {
    cogo_callsched_t callsched;

    // other coroutines running concurrently
    COGO_CQ_T cq;

    // global:
    //  struct cogo_paralsched* run; // running schedulers (idles not in list)
};

COGO_PARAL_T* cogo_paralsched_resume(COGO_PARALSCHED_T* paralsched);
int cogo_paralsched_add(COGO_PARALSCHED_T* paralsched, COGO_PARAL_T* paral);
COGO_PARAL_T* cogo_paralsched_remove(COGO_PARALSCHED_T* paralsched);

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

#define COGO_MSG_T           cogo_msg_t
#define COGO_MSG_NEXT(MSG)   ((MSG)->next)

#define COGO_QUEUE_VALUE_T   COGO_MSG_T
#define COGO_QUEUE_NEXT(MSG) COGO_MSG_NEXT(MSG)
#include "private/cogo_queue_template.h"
#undef COGO_QUEUE_VALUE_T
#undef COGO_QUEUE_NEXT
#define COGO_MQ_T               COGO_QUEUE_T(COGO_MSG_T)
#define COGO_MQ_MAKE            COGO_QUEUE_MAKE(COGO_MSG_T)
#define COGO_MQ_ADD             COGO_QUEUE_ADD(COGO_MSG_T)
#define COGO_MQ_REMOVE          COGO_QUEUE_REMOVE(COGO_MSG_T)
#define COGO_MQ_REMOVE_NONEMPTY COGO_QUEUE_REMOVE_NONEMPTY(COGO_MSG_T)
typedef struct cogo_chan {
    // all coroutines blocked by this channel
    COGO_CQ_T cq;

    // message queue
    COGO_MQ_T mq;

    // current size
    // < 0  : some readers blocked
    // > max_size: some writers blocked
    long size;

    // capacity
    long max_size;
} cogo_chan_t;

#define COGO_CHAN_T                          cogo_chan_t
#define COGO_CHAN_ADD_MSG(CHAN, MSG)         COGO_MQ_ADD(&(CHAN)->mq, (MSG))
#define COGO_CHAN_REMOVE_MSG(CHAN)           COGO_MQ_REMOVE(&(CHAN)->mq)
#define COGO_CHAN_REMOVE_MSG_NONEMPTY(CHAN)  COGO_MQ_REMOVE_NONEMPTY(&(CHAN)->mq)
#define COGO_CHAN_ADD_COGO(CHAN, COGO)       COGO_CQ_ADD(&(CHAN)->cq, (COGO))
#define COGO_CHAN_REMOVE_COGO(CHAN)          COGO_CQ_REMOVE(&(CHAN)->cq)
#define COGO_CHAN_REMOVE_COGO_NONEMPTY(CHAN) COGO_CQ_REMOVE_NONEMPTY(&(CHAN)->cq)
#define COGO_CHAN_SIZE(CHAN)                 ((CHAN)->size)
#define COGO_CHAN_MAXSIZE(CHAN)              ((CHAN)->max_size)

// Switch context if return non-zero.
int cogo_chan_read(COGO_PARAL_T* cogo, COGO_CHAN_T* chan, COGO_MSG_T* msg_next);

// Switch context if return non-zero.
int cogo_chan_write(COGO_PARAL_T* cogo, COGO_CHAN_T* chan, COGO_MSG_T* msg);

#define COGO_PARAL_INIT(COGO, FUNC)               \
    {                                             \
            /*.call=*/COGO_CALL_INIT(COGO, FUNC), \
            /*.next=*/0,                          \
    }
#define COGO_PARAL_PT(PARAL)                 COGO_CALL_PT(COGO_PARAL_CALL(PARAL))
#define COGO_PARAL_TOP(PARAL)                COGO_CALL_TOP(COGO_PARAL_CALL(PARAL))
#define COGO_PARAL_SCHED(PARAL)              COGO_CALL_SCHED(COGO_PARAL_CALL(PARAL))
#define COGO_PARAL_CALL(PARAL)               (&(PARAL)->call)
#define COGO_PARAL_NEXT(PARAL)               ((PARAL)->next)
#define COGO_PARAL_IS_VALID(PARAL)           COGO_CALL_IS_VALID(COGO_PARAL_CALL(PARAL))
#define COGO_PARAL_BEGIN(PARAL)              COGO_DO_BEGIN(COGO_PARAL_PT(PARAL), PARAL)
#define COGO_PARAL_YIELD(PARAL)              COGO_DO_YIELD(COGO_PARAL_PT(PARAL), COGO_ON_YIELD, COGO_ON_RESUME, PARAL)
#define COGO_PARAL_RETURN(PARAL)             COGO_DO_RETURN(PARAL)
#define COGO_PARAL_END(PARAL)                COGO_DO_END(COGO_PARAL_PT(PARAL), PARAL)
#define COGO_PARAL_STATUS(PARAL)             COGO_PT_STATUS(COGO_PARAL_PT(PARAL))
#define COGO_PARAL_AWAIT(PARAL, PARAL_OTHER) COGO_DO_AWAIT(COGO_PARAL_CALL(PARAL), COGO_PARAL_CALL(PARAL_OTHER), PARAL, PARAL_OTHER)
#define COGO_PARAL_RESUME(PARAL)             cogo_paral_resume(PARAL)
#define COGO_PARAL_ASYNC(PARAL, PARAL_OTHER) COGO_DO_ASYNC(PARAL, PARAL_OTHER, PARAL, PARAL_OTHER)
#define COGO_PARAL_RUN(PARAL)                cogo_paral_run(PARAL)

#undef COGO_INIT
#undef COGO_CALL_OF
#undef COGO_IS_VALID
#undef COGO_RESUME

#define COGO_INIT(COGO, FUNC)        COGO_PARAL_INIT(COGO, FUNC)
#define COGO_PARAL_OF(COGO)          (COGO)
#define COGO_IS_VALID(COGO)          ((COGO) == (COGO) && (COGO) && COGO_PARAL_IS_VALID(COGO))
#define COGO_RESUME(COGO)            COGO_PARAL_RESUME(COGO)
#define COGO_RUN(COGO)               COGO_PARAL_RUN(COGO)
#define COGO_CALL_OF(COGO)           COGO_PARAL_CALL(COGO_PARAL_OF(COGO))
#define COGO_NEXT_OF(COGO)           COGO_PARAL_NEXT(COGO_PARAL_OF(COGO))

/// Start a new coroutine to run concurrently.
#define COGO_ASYNC(COGO, COGO_OTHER) COGO_DO_ASYNC(COGO_PARAL_OF(COGO), COGO_PARAL_OF(COGO_OTHER), COGO, COGO_OTHER)

#define COGO_PARALSCHED_INIT(COGO)                     \
    {                                                  \
            /*.call_sched=*/COGO_CALLSCHED_INIT(COGO), \
            /*.cq=*/COGO_QUEUE_INITIALIZER(),          \
    }
#define COGO_PARALSCHED_TOP(PARALSCHED)        COGO_CALLSCHED_TOP(COGO_PARALSCHED_CALLSCHED(PARALSCHED))
#define COGO_PARALSCHED_CALLSCHED(PARALSCHED)  (&(PARALSCHED)->callsched)
#define COGO_PARALSCHED_CQ(PARALSCHED)         (&(PARALSCHED)->cq)
#define COGO_PARALSCHED_IS_VALID(PARALSCHED)   COGO_CALLSCHED_IS_VALID(COGO_PARALSCHED_CALLSCHED(PARALSCHED))
#define COGO_PARALSCHED_RESUME(PARALSCHED)     cogo_paralsched_resume(PARALSCHED)
#define COGO_PARALSCHED_ADD(PARALSCHED, PARAL) cogo_paralsched_add(PARALSCHED, PARAL)
#define COGO_PARALSCHED_REMOVE(PARALSCHED)     cogo_paralsched_remove(PARALSCHED)

#undef COGO_SCHED_INIT
#undef COGO_SCHED_CALLSCHED
#undef COGO_SCHED_IS_VALID
#undef COGO_SCHED_RESUME

#define COGO_SCHED_INIT(COGO)        COGO_PARALSCHED_INIT(COGO)
#define COGO_SCHED_PARALSCHED(SCHED) (SCHED)
#define COGO_SCHED_IS_VALID(SCHED)   ((SCHED) == (SCHED) && (SCHED) && COGO_PARALSCHED_IS_VALID(SCHED))
#define COGO_SCHED_RESUME(SCHED)     COGO_PARALSCHED_RESUME(SCHED)

#define COGO_SCHED_CALLSCHED(SCHED)  COGO_PARALSCHED_CALLSCHED(COGO_SCHED_PARALSCHED(SCHED))
#define COGO_SCHED_CQ(SCHED)         COGO_PARALSCHED_CQ(COGO_SCHED_PARALSCHED(SCHED))

#define COGO_CHAN_INIT(N)                     \
    {                                         \
            /*.cq=*/COGO_QUEUE_INITIALIZER(), \
            /*.mq=*/COGO_QUEUE_INITIALIZER(), \
            /*.size=*/0,                      \
            /*.max_size=*/(N),                \
    }

#define COGO_CHAN_IS_VALID(CHAN)             ((CHAN) == (CHAN) && (CHAN) && COGO_CHAN_MAXSIZE(CHAN) >= 0)

/** Receive a message from channel.
 If there's no message in channel, block until one arrived.

 @param[in] CHAN the channel where to receive message.
 @param[out] MSG_NEXT the address of received message is stored in MSG_NEXT->next.

 @pre This macro can only be called in the coroutine function, i.e. between CO_BEGIN and CO_END.
 @pre `(COGO) != nullptr && !__has_side_effects(COGO)`
 @pre `(CHAN) != nullptr && !__has_side_effects(CHAN)`
 @pre `(MSG_NEXT) != nullptr && !__has_side_effects(MSG_NEXT)`
 @post At the time of read success (and before any other messages written by other coroutines), the size of channel is decreased by 1.
 @post MSG_NEXT->next != nullptr

 @par Example
 @code
CO_DECLARE(coro_t, COGO_CHAN_T* chan, COGO_MSG_T msg) {
  coro_t* thiz = (coro_t*)COGO_THIS;
  COGO_CHAN_T* chan = thiz->chan;
  COGO_MSG_T* msg = &thiz->msg;
CO_BEGIN:

  CO_CHAN_READ(chan, msg);
  T* obj = (T*)msg->next;

CO_END:;
}
 @endcode
*/
#define COGO_CHAN_READ(COGO, CHAN, MSG_NEXT) COGO_DO_CHAN_READ(COGO_PARAL_OF(COGO), CHAN, MSG_NEXT, COGO)

/** Send a message through channel. If channel is full, block until available.
 @param[in] CHAN the channel where to send message.
 @param[out] MSG the message to send.
 @pre This macro can only be called in the coroutine function, i.e. between CO_BEGIN and CO_END, or COGO_BEGIN() and COGO_END()
 @pre `(COGO) != nullptr && !__has_side_effects(COGO)`
 @pre `(CHAN) != nullptr && !__has_side_effects(CHAN)`
 @pre `(MSG) != nullptr && !__has_side_effects(MSG)`
 @post At the time of writing success (and before any message has been read by other coroutines), the size of channel is increased by 1.
 @invariant the MSG body is not modified.
*/
#define COGO_CHAN_WRITE(COGO, CHAN, MSG)     COGO_DO_CHAN_WRITE(COGO_PARAL_OF(COGO), CHAN, MSG, COGO)

/// Abbr of `COGO_ASYNC(COGO_THIS, COGO_OTHER)`.
#define CO_ASYNC(COGO_OTHER)                 COGO_ASYNC(COGO_THIS, COGO_OTHER)
/// Abbr of `COGO_CHAN_READ(COGO_THIS, CHAN, MSG_NEXT)`.
#define CO_CHAN_READ(CHAN, MSG_NEXT)         COGO_CHAN_READ(COGO_THIS, CHAN, MSG_NEXT)
/// Abbr of `COGO_CHAN_WRITE(COGO_THIS, CHAN, MSG)`.
#define CO_CHAN_WRITE(CHAN, MSG)             COGO_CHAN_WRITE(COGO_THIS, CHAN, MSG)

#define COGO_DO_ASYNC(PARAL, PARAL_OTHER, COGO, COGO_OTHER)                  \
    do {                                                                     \
        COGO_BEFORE_ASYNC((+(COGO)), (+(COGO_OTHER)));                       \
                                                                             \
        if (COGO_SCHED_ADD(COGO_PARAL_SCHED(PARAL), PARAL_OTHER)) {          \
            COGO_DO_YIELD(COGO_PARAL_PT(PARAL), COGO_NOOP, COGO_NOOP, COGO); \
        }                                                                    \
                                                                             \
        COGO_AFTER_ASYNC((+(COGO)), (+(COGO_OTHER)));                        \
    } while (0)

#define COGO_DO_CHAN_READ(PARAL, CHAN, MSG_NEXT, COGO)                       \
    do {                                                                     \
        COGO_BEFORE_CHAN_READ((+(COGO)), (+(CHAN)), (+(MSG_NEXT)));          \
                                                                             \
        if (cogo_chan_read(PARAL, (CHAN), (MSG_NEXT))) {                     \
            COGO_DO_YIELD(COGO_PARAL_PT(PARAL), COGO_NOOP, COGO_NOOP, COGO); \
        }                                                                    \
                                                                             \
        COGO_AFTER_CHAN_READ((+(COGO)), (+(CHAN)), (+(MSG_NEXT)));           \
    } while (0)

#define COGO_DO_CHAN_WRITE(PARAL, CHAN, MSG, COGO)                           \
    do {                                                                     \
        COGO_BEFORE_CHAN_WRITE((+(COGO)), (+(CHAN)), (+(MSG)));              \
                                                                             \
        if (cogo_chan_write(PARAL, (CHAN), (MSG))) {                         \
            COGO_DO_YIELD(COGO_PARAL_PT(PARAL), COGO_NOOP, COGO_NOOP, COGO); \
        }                                                                    \
                                                                             \
        COGO_AFTER_CHAN_WRITE((+(COGO)), (+(CHAN)), (+(MSG)));               \
    } while (0)

#ifdef __cplusplus
}
#endif
#endif  // COGO_PARAL_H_
