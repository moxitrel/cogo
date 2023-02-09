#include <cogo/co_st.h>

// implement cogo_sch_push()
int cogo_sch_push(cogo_sch_t* sch, cogo_co_t* co) {
  COGO_ASSERT(sch);
  COGO_ASSERT(co);
  COGO_QUEUE_PUSH(co_t)
  (&((co_sch_t*)sch)->q, (co_t*)co);
  return 1;  // switch context
}

// implement cogo_sch_pop()
cogo_co_t* cogo_sch_pop(cogo_sch_t* sch) {
  COGO_ASSERT(sch);
  return (cogo_co_t*)COGO_QUEUE_POP(co_t)(&((co_sch_t*)sch)->q);
}

int cogo_chan_read(co_t* co, co_chan_t* chan, co_msg_t* msg_next) {
  COGO_ASSERT(co);
  COGO_ASSERT(chan);
  COGO_ASSERT(chan->cap >= 0);
  COGO_ASSERT(chan->size > PTRDIFF_MIN);
  COGO_ASSERT(msg_next);

  const ptrdiff_t chan_size = chan->size--;
  if (chan_size <= 0) {
    COGO_QUEUE_PUSH(co_msg_t)
    (&chan->mq, msg_next);
    // sleep in background
    COGO_QUEUE_PUSH(co_t)
    (&chan->cq, co);                          // append to blocking queue
    ((cogo_co_t*)co)->sch->stack_top = NULL;  // remove from scheduler
    return 1;
  } else {
    msg_next->next = COGO_QUEUE_POP_NONEMPTY(co_msg_t)(&chan->mq);
    if (chan_size < chan->cap) {
      return 0;
    }
    // wake up a writer if exists
    cogo_co_t* writer = (cogo_co_t*)COGO_QUEUE_POP_NONEMPTY(co_t)(&chan->cq);
    return cogo_sch_push(((cogo_co_t*)co)->sch, writer);
  }
}

int cogo_chan_write(co_t* co, co_chan_t* chan, co_msg_t* msg) {
  COGO_ASSERT(co);
  COGO_ASSERT(chan);
  COGO_ASSERT(chan->cap >= 0);
  COGO_ASSERT(chan->size < PTRDIFF_MAX);
  COGO_ASSERT(msg);

  const ptrdiff_t chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_QUEUE_POP_NONEMPTY(co_msg_t)
    (&chan->mq)->next = msg;
    // wake up a reader
    cogo_co_t* reader = (cogo_co_t*)COGO_QUEUE_POP_NONEMPTY(co_t)(&chan->cq);
    return cogo_sch_push(((cogo_co_t*)co)->sch, reader);
  } else {
    COGO_QUEUE_PUSH(co_msg_t)
    (&chan->mq, msg);
    if (chan_size < chan->cap) {
      return 0;
    }
    // sleep in background
    COGO_QUEUE_PUSH(co_t)
    (&chan->cq, co);
    ((cogo_co_t*)co)->sch->stack_top = NULL;
    return 1;
  }
}
