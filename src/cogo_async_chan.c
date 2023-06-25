#include <cogo/cogo_async.h>
#include <limits.h>

int cogo_chan_read(cogo_async_t* const co_this, co_chan_t* const chan, co_message_t* const msg_next) {
#define SCHED (((cogo_await_t*)co_this)->sched)
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);

  const ptrdiff_t chan_size = chan->size--;
  if (chan_size <= 0) {
    COGO_MQ_PUSH(&chan->mq, msg_next);
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, co_this);  // append to blocking queue
    SCHED->call_top = NULL;            // remove from scheduler
    return 1;                          // switch context
  } else {
    msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
    if (chan_size < chan->cap) {
      return 0;
    }
    // wake up a writer if exists
    return cogo_async_sched_push((cogo_async_sched_t*)SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
  }

#undef SCHED
}

int cogo_chan_write(cogo_async_t* const co_this, co_chan_t* const chan, co_message_t* const msg) {
#define SCHED (((cogo_await_t*)co_this)->sched)
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);

  const ptrdiff_t chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
    // wake up a reader
    return cogo_async_sched_push((cogo_async_sched_t*)SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
  } else {
    COGO_MQ_PUSH(&chan->mq, msg);
    if (chan_size < chan->cap) {
      return 0;
    }
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, co_this);
    SCHED->call_top = NULL;
    return 1;
  }

#undef SCHED
}
