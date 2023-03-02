#include <cogo/cogo_co.h>
#include <limits.h>

// implement cogo_await_sched_push()
int cogo_await_sched_push(cogo_await_sched_t* const sched, cogo_await_t* const co) {
  COGO_ASSERT(sched && co);
  (COGO_QUEUE_PUSH(cogo_co_t)(&((cogo_co_sched_t*)sched)->q, (cogo_co_t*)co));
  return 1;  // switch context
}

// implement cogo_await_sched_pop()
cogo_await_t* cogo_await_sched_pop(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched);
  return (cogo_await_t*)COGO_QUEUE_POP(cogo_co_t)(&((cogo_co_sched_t*)sched)->q);
}

int cogo_chan_read(cogo_co_t* const co_this, co_chan_t* const chan, co_msg_t* const msg_next) {
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);

  const ptrdiff_t chan_size = chan->size--;
  if (chan_size <= 0) {
    (COGO_QUEUE_PUSH(co_msg_t)(&chan->mq, msg_next));
    // sleep in background
    (COGO_QUEUE_PUSH(cogo_co_t)(&chan->cq, co_this));  // append to blocking queue
    co_this->super.sched->stack_top = NULL;            // remove from scheduler
    return 1;
  } else {
    msg_next->next = COGO_QUEUE_POP_NONEMPTY(co_msg_t)(&chan->mq);
    if (chan_size < chan->cap) {
      return 0;
    }
    // wake up a writer if exists
    cogo_await_t* writer = (cogo_await_t*)COGO_QUEUE_POP_NONEMPTY(cogo_co_t)(&chan->cq);
    return cogo_await_sched_push(co_this->super.sched, writer);
  }
}

int cogo_chan_write(cogo_co_t* const co_this, co_chan_t* const chan, co_msg_t* const msg) {
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);

  const ptrdiff_t chan_size = chan->size++;
  if (chan_size < 0) {
    (COGO_QUEUE_POP_NONEMPTY(co_msg_t)(&chan->mq))->next = msg;
    // wake up a reader
    cogo_await_t* reader = (cogo_await_t*)COGO_QUEUE_POP_NONEMPTY(cogo_co_t)(&chan->cq);
    return cogo_await_sched_push(co_this->super.sched, reader);
  } else {
    (COGO_QUEUE_PUSH(co_msg_t)(&chan->mq, msg));
    if (chan_size < chan->cap) {
      return 0;
    }
    // sleep in background
    (COGO_QUEUE_PUSH(cogo_co_t)(&chan->cq, co_this));
    co_this->super.sched->stack_top = NULL;
    return 1;
  }
}
