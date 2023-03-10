#include <cogo/cogo_co.h>
#include <limits.h>

#ifdef __GNUC__
#define COGO_FALLTHROUGH __attribute__((fallthrough))
#else
#define COGO_FALLTHROUGH /* fallthrough */
#endif

// run until yield
cogo_co_t* cogo_co_sched_step(cogo_co_sched_t* const sched) {
#define STACK_TOP sched->super.call_top
  COGO_ASSERT(sched && STACK_TOP);

  for (;;) {
    STACK_TOP->sched = &sched->super;
    STACK_TOP->func(STACK_TOP);
    if (!STACK_TOP) {
      // blocked
      if (!(STACK_TOP = &cogo_co_sched_pop(sched)->super)) {
        // no more active coroutines
        goto exit;
      }
      continue;
    }
    switch (cogo_status(STACK_TOP)) {
      case CO_STATUS_FINI:  // return
        if (!cogo_await_return(STACK_TOP)) {
          // return from root
          goto exit_next;
        }
        COGO_FALLTHROUGH;
      case CO_STATUS_INIT:  // await
        continue;
      default:  // yield
        cogo_co_sched_push(sched, (cogo_co_t*)STACK_TOP);
        goto exit_next;
    }
  }
exit_next:
  STACK_TOP = &cogo_co_sched_pop(sched)->super;
exit:
  return (cogo_co_t*)STACK_TOP;

#undef STACK_TOP
}

int cogo_chan_read(cogo_co_t* const co_this, co_chan_t* const chan, co_msg_t* const msg_next) {
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);
  const ptrdiff_t chan_size = chan->size--;
  if (chan_size <= 0) {
    COGO_MQ_PUSH(&chan->mq, msg_next);
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, co_this);       // append to blocking queue
    co_this->super.sched->call_top = NULL;  // remove from scheduler
    return 1;                               // switch context
  } else {
    msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
    if (chan_size < chan->cap) {
      return 0;
    }
    // wake up a writer if exists
    cogo_co_t* writer = COGO_CQ_POP_NONEMPTY(&chan->cq);
    return cogo_co_sched_push((cogo_co_sched_t*)co_this->super.sched, writer);
  }
}

int cogo_chan_write(cogo_co_t* const co_this, co_chan_t* const chan, co_msg_t* const msg) {
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);
  const ptrdiff_t chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
    // wake up a reader
    cogo_co_t* reader = COGO_CQ_POP_NONEMPTY(&chan->cq);
    return cogo_co_sched_push((cogo_co_sched_t*)co_this->super.sched, reader);
  } else {
    COGO_MQ_PUSH(&chan->mq, msg);
    if (chan_size < chan->cap) {
      return 0;
    }
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, co_this);
    co_this->super.sched->call_top = NULL;
    return 1;
  }
}
