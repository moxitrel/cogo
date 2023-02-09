/*

* API
CO_BEGIN                : ...
CO_END                  : ...
CO_YIELD                : ...
CO_RETURN               : ...
co_this                 : ...
co_status   (CO)        : ...
CO_DECLARE  (NAME, ...) : ...
CO_DEFINE   (NAME)      : ...
CO_MAKE     (NAME, ...) : ...
CO_AWAIT    (cogo_co_t*): ...
CO_START    (cogo_co_t*): ...

co_t                                    : coroutine type to be inherited
co_run      (co_t*)                     : run the coroutine until all finished

co_msg_t                                : channel message type
co_chan_t                               : channel type
CO_CHAN_MAKE (size_t)                   : return a channel with capacity size_t
CO_CHAN_WRITE(co_chan_t*, co_msg_t*)    : send a message to channel
CO_CHAN_READ (co_chan_t*, co_msg_t*)    : receive a message from channel, the result stored in co_msg_t.next

*/
#ifndef COGO_CO_IMPL_H_
#define COGO_CO_IMPL_H_

#include <stddef.h>

#include "cogo_co.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct co co_t;
typedef struct co_sch co_sch_t;
typedef struct co_msg co_msg_t;

struct co {
  // inherit cogo_co_t
  cogo_co_t cogo_co;

  // build coroutine queue
  co_t* next;
};

//
// COGO_QUEUE_T             (co_t)
// COGO_QUEUE_IS_EMPTY      (co_t) (const COGO_QUEUE_T(co_t)*)
// COGO_QUEUE_POP           (co_t) (      COGO_QUEUE_T(co_t)*)
// COGO_QUEUE_POP_NONEMPTY  (co_t) (      COGO_QUEUE_T(co_t)*)
// COGO_QUEUE_PUSH          (co_t) (      COGO_QUEUE_T(co_t)*, co_t*)
//
#undef COGO_QUEUE_ITEM_T
#undef COGO_QUEUE_ITEM_NEXT
#define COGO_QUEUE_ITEM_T       co_t
#define COGO_QUEUE_ITEM_NEXT(P) ((P)->next)
#include "co_st_q.inc"
struct co_sch {
  // inherent cogo_sch_t
  cogo_sch_t cogo_sch;

  // coroutine queue run concurrently
  COGO_QUEUE_T(co_t)
  q;
};

static inline void co_run(void* co) {
  co_sch_t sch = {
      .cogo_sch = {
          .stack_top = (cogo_co_t*)co,
      },
  };
  while (cogo_sch_step((cogo_sch_t*)&sch)) {
    // noop
  }
}

// channel message
struct co_msg {
  co_msg_t* next;
};

//
// COGO_QUEUE_T             (co_msg_t)
// COGO_QUEUE_IS_EMPTY         (co_msg_t) (const COGO_QUEUE_T(co_msg_t)*)
// COGO_QUEUE_POP           (co_msg_t) (      COGO_QUEUE_T(co_msg_t)*)
// COGO_QUEUE_POP_NONEMPTY  (co_msg_t) (      COGO_QUEUE_T(co_msg_t)*)
// COGO_QUEUE_PUSH          (co_msg_t) (      COGO_QUEUE_T(co_msg_t)*, co_msg_t*)
//
#undef COGO_QUEUE_ITEM_T
#undef COGO_QUEUE_ITEM_NEXT
#define COGO_QUEUE_ITEM_T       co_msg_t
#define COGO_QUEUE_ITEM_NEXT(P) ((P)->next)
#include "co_st_q.inc"
typedef struct co_chan {
  // all coroutines blocked by this channel
  COGO_QUEUE_T(co_t)
  cq;

  // message queue
  COGO_QUEUE_T(co_msg_t)
  mq;

  // current size
  ptrdiff_t size;

  // max size
  ptrdiff_t cap;
} co_chan_t;

#define CO_CHAN_MAKE(N) ((co_chan_t){.cap = (N)})

// CO_CHAN_READ(co_chan_t*, co_msg_t*);
// MSG_NEXT: the read message sit in MSG_NEXT->next
#define CO_CHAN_READ(CHAN, MSG_NEXT)                               \
  do {                                                             \
    if (cogo_chan_read((co_t*)co_this, (CHAN), (MSG_NEXT)) != 0) { \
      CO_YIELD;                                                    \
    }                                                              \
  } while (0)
int cogo_chan_read(co_t* co, co_chan_t* chan, co_msg_t* msg_next);

// CO_CHAN_WRITE(co_chan_t*, co_msg_t*);
#define CO_CHAN_WRITE(CHAN, MSG)                                          \
  do {                                                                    \
    if (cogo_chan_write((co_t*)co_this, (CHAN), (co_msg_t*)(MSG)) != 0) { \
      CO_YIELD;                                                           \
    }                                                                     \
  } while (0)
int cogo_chan_write(co_t* co, co_chan_t* chan, co_msg_t* msg);

#undef CO_DECLARE
#define CO_DECLARE(NAME, ...) \
  COGO_DECLARE(NAME, co_t co, __VA_ARGS__)

#undef CO_MAKE
#define CO_MAKE(NAME, ...) \
  ((NAME##_t){{.cogo_co = {.func = NAME##_func}}, __VA_ARGS__})

#ifdef __cplusplus
}
#endif
#endif /* COGO_CO_IMPL_H_ */
