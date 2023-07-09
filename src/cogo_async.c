#include <cogo/cogo_async.h>
#include <limits.h>

// run until yield
// NOTE: .entry is damaged after call
cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched) {
#define CALL_TOP (((cogo_await_sched_t*)sched)->call_top)
  COGO_ASSERT(sched && CALL_TOP);

  for (;;) {
    CALL_TOP->sched = (cogo_await_sched_t*)sched;
    CALL_TOP->func(CALL_TOP);
    if (!CALL_TOP) {
      // blocked
      if (!(CALL_TOP = (cogo_await_t*)cogo_async_sched_pop(sched))) {
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
        cogo_async_sched_push(sched, (cogo_async_t*)CALL_TOP);
        goto exit_next;
    }
  }
exit_next:
  CALL_TOP = (cogo_await_t*)cogo_async_sched_pop(sched);
exit:
  return (cogo_async_t*)CALL_TOP;

#undef CALL_TOP
}

#ifndef COGO_NO_RESUME
cogo_async_t* cogo_async_resume(cogo_async_t* const co) {
#define ENTRY (((cogo_await_t*)co)->entry)
  COGO_ASSERT(co);
  cogo_async_sched_t sched = {
      .super = {
          .call_top = ENTRY ? ENTRY : (cogo_await_t*)co,
      },
  };
  // repaire .entry
  return (cogo_async_t*)(ENTRY = (cogo_await_t*)cogo_async_sched_resume(&sched));
#undef ENTRY
}
#endif

int cogo_chan_read(cogo_async_t* const co_this, co_chan_t* const chan, co_message_t* const msg_next) {
#define SCHED (((cogo_await_t*)co_this)->sched)
  COGO_ASSERT(co_this && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);

  ptrdiff_t const chan_size = chan->size--;
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

  ptrdiff_t const chan_size = chan->size++;
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

int cogo_async_sched_push(cogo_async_sched_t* const sched, cogo_async_t* const co) {
  COGO_ASSERT(sched && co);
  COGO_CQ_PUSH(&sched->q, co);
  return 1;  // switch context
}

cogo_async_t* cogo_async_sched_pop(cogo_async_sched_t* const sched) {
  COGO_ASSERT(sched);
  return COGO_CQ_POP(&sched->q);
}

void cogo_async_run(cogo_async_t* co) {
  COGO_ASSERT(co);
  cogo_async_sched_t sched = {
      .super = {
          .call_top = (cogo_await_t*)(co),
      },
  };
  while (cogo_async_sched_resume(&sched)) {
    // noop
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
        break;
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
