#include <cogo/cogo_co.h>
#include <limits.h>

#ifdef __GNUC__
#define COGO_FALLTHROUGH __attribute__((fallthrough))
#else
#define COGO_FALLTHROUGH /* [[fallthrough]] */
#endif

// run until yield
cogo_co_t* cogo_co_sched_step(cogo_co_sched_t* sched) {
  COGO_ASSERT(sched && sched->super.stack_top);
  for (;;) {
    sched->super.stack_top->sched = &sched->super;
    sched->super.stack_top->func(sched->super.stack_top);
    if (!sched->super.stack_top) {
      // blocked
      if (!(sched->super.stack_top = &cogo_co_sched_pop(sched)->super)) {
        // no more active coroutines
        goto exit;
      }
      continue;
    }
    switch (co_status(sched->super.stack_top)) {
      case CO_STATUS_FINI:  // return
        if (!cogo_await_return(sched->super.stack_top)) {
          // return from root
          goto exit_next;
        }
        COGO_FALLTHROUGH;
      case CO_STATUS_INIT:  // await
        continue;
      default:  // yield
        cogo_co_sched_push(sched, (cogo_co_t*)sched->super.stack_top);
        goto exit_next;
    }
  }
exit_next:
  sched->super.stack_top = &cogo_co_sched_pop(sched)->super;
exit:
  return (cogo_co_t*)sched->super.stack_top;
}

int cogo_chan_read(cogo_co_t* const co_this, co_chan_t* const chan, co_msg_t* const msg_next) {
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);

  const ptrdiff_t chan_size = chan->size--;
  if (chan_size <= 0) {
    (COGO_QUEUE_PUSH(co_msg_t)(&chan->mq, msg_next));
    // sleep in background
    (COGO_QUEUE_PUSH(cogo_co_t)(&chan->cq, co_this));  // append to blocking queue
    co_this->super.sched->stack_top = NULL;            // remove from scheduler
    return 1;                                          // switch context
  } else {
    msg_next->next = COGO_QUEUE_POP_NONEMPTY(co_msg_t)(&chan->mq);
    if (chan_size < chan->cap) {
      return 0;
    }
    // wake up a writer if exists
    cogo_co_t* writer = COGO_QUEUE_POP_NONEMPTY(cogo_co_t)(&chan->cq);
    return cogo_co_sched_push((cogo_co_sched_t*)co_this->super.sched, writer);
  }
}

int cogo_chan_write(cogo_co_t* const co_this, co_chan_t* const chan, co_msg_t* const msg) {
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);

  const ptrdiff_t chan_size = chan->size++;
  if (chan_size < 0) {
    (COGO_QUEUE_POP_NONEMPTY(co_msg_t)(&chan->mq))->next = msg;
    // wake up a reader
    cogo_co_t* reader = COGO_QUEUE_POP_NONEMPTY(cogo_co_t)(&chan->cq);
    return cogo_co_sched_push((cogo_co_sched_t*)co_this->super.sched, reader);
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
