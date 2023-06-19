#include <cogo/cogo_co.h>
#include <limits.h>

// run until yield
cogo_co_t* cogo_co_sched_resume(cogo_co_sched_t* const sched) {
#define CALL_TOP (((cogo_await_sched_t*)sched)->call_top)
  COGO_ASSERT(sched && CALL_TOP);

  for (;;) {
    CALL_TOP->sched = (cogo_await_sched_t*)sched;
    CALL_TOP->func(CALL_TOP);
    if (!CALL_TOP) {
      // blocked
      if (!(CALL_TOP = (cogo_await_t*)cogo_co_sched_pop(sched))) {
        // no more active coroutines
        goto exit;
      }
      continue;
    }
    switch (co_status((cogo_yield_t*)CALL_TOP)) {
      case CO_STATUS_END:  // return
        if (!(CALL_TOP = CALL_TOP->caller)) {
          // return from root
          goto exit_next;
        }
        continue;
      case CO_STATUS_BEGIN:  // await
        continue;
      default:  // yield
        cogo_co_sched_push(sched, (cogo_co_t*)CALL_TOP);
        goto exit_next;
    }
  }
exit_next:
  CALL_TOP = (cogo_await_t*)cogo_co_sched_pop(sched);
exit:
  return (cogo_co_t*)CALL_TOP;

#undef CALL_TOP
}

cogo_co_t* cogo_co_resume(cogo_co_t* const co) {
#define ENTRY (((cogo_await_t*)co)->entry)
  COGO_ASSERT(co);

  cogo_co_sched_t sched = {
      .super = {
          .call_top = ENTRY ? ENTRY : (cogo_await_t*)co,
      },
  };
  return (cogo_co_t*)(ENTRY = (cogo_await_t*)cogo_co_sched_resume(&sched));

#undef ENTRY
}

int cogo_chan_read(cogo_co_t* const co_this, co_channel_t* const chan, co_message_t* const msg_next) {
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
    return cogo_co_sched_push((cogo_co_sched_t*)SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
  }

#undef SCHED
}

int cogo_chan_write(cogo_co_t* const co_this, co_channel_t* const chan, co_message_t* const msg) {
#define SCHED (((cogo_await_t*)co_this)->sched)
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);

  const ptrdiff_t chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
    // wake up a reader
    return cogo_co_sched_push((cogo_co_sched_t*)SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
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
