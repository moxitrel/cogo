#include <cogo/cogo_st.h>
#include <limits.h>

// implement cogo_sch_push()
int cogo_sch_push(cogo_await_sch_t* const sch, cogo_await_t* const co) {
  COGO_ASSERT(sch);
  COGO_ASSERT(co);
  (COGO_QUEUE_PUSH(cogo_st_t)(&((cogo_st_sch_t*)sch)->q, (cogo_st_t*)co));
  return 1;  // switch context
}

// implement cogo_sch_pop()
cogo_await_t* cogo_sch_pop(cogo_await_sch_t* const sch) {
  COGO_ASSERT(sch);
  return (cogo_await_t*)COGO_QUEUE_POP(cogo_st_t)(&((cogo_st_sch_t*)sch)->q);
}

int cogo_chan_read(cogo_st_t* const co, co_chan_t* const chan, co_msg_t* const msg_next) {
  COGO_ASSERT(co);
  COGO_ASSERT(chan);
  COGO_ASSERT(chan->cap >= 0);
  COGO_ASSERT(chan->size > PTRDIFF_MIN);
  COGO_ASSERT(msg_next);

  const ptrdiff_t chan_size = chan->size--;
  if (chan_size <= 0) {
    (COGO_QUEUE_PUSH(co_msg_t)(&chan->mq, msg_next));
    // sleep in background
    (COGO_QUEUE_PUSH(cogo_st_t)(&chan->cq, co));  // append to blocking queue
    co->super.sch->stack_top = NULL;              // remove from scheduler
    return 1;
  } else {
    msg_next->next = COGO_QUEUE_POP_NONEMPTY(co_msg_t)(&chan->mq);
    if (chan_size < chan->cap) {
      return 0;
    }
    // wake up a writer if exists
    cogo_await_t* writer = (cogo_await_t*)COGO_QUEUE_POP_NONEMPTY(cogo_st_t)(&chan->cq);
    return cogo_sch_push(co->super.sch, writer);
  }
}

int cogo_chan_write(cogo_st_t* const co, co_chan_t* const chan, co_msg_t* const msg) {
  COGO_ASSERT(co);
  COGO_ASSERT(chan);
  COGO_ASSERT(chan->cap >= 0);
  COGO_ASSERT(chan->size < PTRDIFF_MAX);
  COGO_ASSERT(msg);

  const ptrdiff_t chan_size = chan->size++;
  if (chan_size < 0) {
    (COGO_QUEUE_POP_NONEMPTY(co_msg_t)(&chan->mq))->next = msg;
    // wake up a reader
    cogo_await_t* reader = (cogo_await_t*)COGO_QUEUE_POP_NONEMPTY(cogo_st_t)(&chan->cq);
    return cogo_sch_push(co->super.sch, reader);
  } else {
    (COGO_QUEUE_PUSH(co_msg_t)(&chan->mq, msg));
    if (chan_size < chan->cap) {
      return 0;
    }
    // sleep in background
    (COGO_QUEUE_PUSH(cogo_st_t)(&chan->cq, co));
    co->super.sch->stack_top = NULL;
    return 1;
  }
}
