#include <cogo/cogo_async.h>
#include <limits.h>
#include <stdbool.h>

// run until yield
cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched) {
  COGO_ASSERT(sched && sched->top);
  for (;;) {
    sched->top->super.sched = sched;
    sched->top->super.super.func(sched->top);
    if (!sched->top) {  // blocked
      sched->top = cogo_async_sched_pop(sched);
      if (!sched->top) {  // no more active coroutines
        goto exit;
      }
      continue;
    }
    switch (CO_STATUS(sched->top)) {
      case CO_STATUS_END:  // return
        sched->top = (cogo_async_t*)sched->top->super.caller;
        if (!sched->top) {  // end
          goto exit_next;
        }
        continue;
      case CO_STATUS_BEGIN:  // await
        continue;
      default:  // yield
        cogo_async_sched_push(sched, sched->top);
        goto exit_next;
    }
  }
exit_next:
  sched->top = cogo_async_sched_pop(sched);
exit:
  return sched->top;
}

co_status_t cogo_async_resume(cogo_async_t* const co) {
  COGO_ASSERT(co);
  if (CO_STATUS(co) != CO_STATUS_END) {
    cogo_async_sched_t sched = {
        .top = co->super.resume ?  (cogo_async_t*)co->super.resume :  co, // resume
    };
    co->super.resume = (cogo_await_t*)cogo_async_sched_resume(&sched);  // save resume point
  }
  return CO_STATUS(co);
}

bool cogo_chan_read(cogo_async_t* const co_this, co_chan_t* const chan, co_message_t* const msg_next) {
#define SCHED ((cogo_async_sched_t*)(co_this->super.sched))
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);

  ptrdiff_t const chan_size = chan->size--;
  if (chan_size <= 0) {
    COGO_MQ_PUSH(&chan->mq, msg_next);
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, co_this);  // append to blocking queue
    SCHED->top = NULL;                 // remove from scheduler
    return true;                       // switch context
  } else {
    msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
    if (chan_size < chan->cap) {
      return false;
    }
    // wake up a writer if exists
    return cogo_async_sched_push(SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
  }

#undef SCHED
}

bool cogo_chan_write(cogo_async_t* const co_this, co_chan_t* const chan, co_message_t* const msg) {
#define SCHED ((cogo_async_sched_t*)(co_this->super.sched))
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);

  ptrdiff_t const chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
    // wake up a reader
    return cogo_async_sched_push(SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
  } else {
    COGO_MQ_PUSH(&chan->mq, msg);
    if (chan_size < chan->cap) {
      return false;
    }
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, co_this);
    SCHED->top = NULL;
    return true;
  }

#undef SCHED
}

bool cogo_async_sched_push(cogo_async_sched_t* const sched, cogo_async_t* const co) {
  COGO_ASSERT(sched && co);
  COGO_CQ_PUSH(&sched->q, co);
  return true;  // switch context
}

cogo_async_t* cogo_async_sched_pop(cogo_async_sched_t* const sched) {
  COGO_ASSERT(sched);
  return COGO_CQ_POP(&sched->q);
}

void cogo_async_run(cogo_async_t* co) {
  COGO_ASSERT(co);
  if (CO_STATUS(co) != CO_STATUS_END) {
    cogo_async_sched_t sched = {
        .top = co->super.resume ?  (cogo_async_t*)co->super.resume :  co,
    };
    while (cogo_async_sched_resume(&sched)) {
    }
  }

  /* mt
  for (;;) {
    if (cogo_async_sched_resume(&sched)) {
      if (has_sleeping_sched() && qsize > 1) {
        wake_sched();
      }
    } else {
      n = steal_timeout(&sched);
      if (n < 0) {
        error;
      }
      if (n == 0) {
        pop_from_run_sched(&sched);
        push_to_sleep_sched(&sched);
        sleep();
      }
    }
  }
  */
}
