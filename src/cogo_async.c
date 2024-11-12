#include <cogo/cogo_async.h>
#include <limits.h>
#include <stdbool.h>

static cogo_async_t* cogo_async_sched_step(cogo_async_sched_t* const sched) {
#define TOP (sched->base_await_sched.top)
  TOP->base_await.sched = sched;
  TOP->base_await.base_yield.resume(TOP);
  if (!(/*blocked*/ !TOP || /*finished*/ (COGO_PC(&TOP->base_await.base_yield.base_pt) == COGO_PC_END && !(TOP = TOP->base_await.caller)))) {
    // yield, await, return, async, read, write
    cogo_async_sched_push(sched, TOP);
  }
  return TOP = cogo_async_sched_pop(sched);
#undef TOP
}

// Run until yield. Return the next coroutine to be run.
static cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched) {
#define TOP (sched->base_await_sched.top)
  COGO_ASSERT(sched && TOP);
  for (;;) {
    TOP->base_await.sched = sched;
    TOP->base_await.base_yield.resume(TOP);
    if (!TOP) {  // blocked
      goto on_blocked;
    } else {
      switch (COGO_PC(&TOP->base_await.base_yield.base_pt)) {
        case COGO_PC_END:  // return
          TOP = TOP->base_await.caller;
          if (!TOP) {  // end
            goto on_end;
          }
          continue;
        case COGO_PC_BEGIN:  // await
          continue;
        default:  // yield, async
          cogo_async_sched_push(sched, TOP);
          TOP = cogo_async_sched_pop(sched);
          goto exit;
      }
    }
  on_blocked:
  on_end:
    TOP = cogo_async_sched_pop(sched);
    if (!TOP) {  // no more active coroutines
      goto exit;
    }
  }
exit:
  return (cogo_async_t*)TOP;
#undef TOP
}

int cogo_chan_read(cogo_async_t* const cogo_this, co_chan_t* const chan, co_message_t* const msg_next) {
  COGO_ASSERT(cogo_this && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);
  ptrdiff_t const chan_size = chan->size--;
  if (chan_size <= 0) {
    COGO_MQ_PUSH(&chan->mq, msg_next);
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, cogo_this);                        // append to blocking queue
    cogo_this->base_await.sched->base_await_sched.top = NULL;  // remove from scheduler
    return true;                                               // switch context
  } else {
    msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
    if (chan_size <= chan->cap) {
      return false;
    } else {
      // wake up a writer
      return cogo_async_sched_push(cogo_this->base_await.sched, COGO_CQ_POP_NONEMPTY(&chan->cq));
    }
  }
}

int cogo_chan_write(cogo_async_t* const cogo_this, co_chan_t* const chan, co_message_t* const msg) {
  COGO_ASSERT(cogo_this && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);
  ptrdiff_t const chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
    // wake up a reader
    return cogo_async_sched_push(cogo_this->base_await.sched, COGO_CQ_POP_NONEMPTY(&chan->cq));
  } else {
    COGO_MQ_PUSH(&chan->mq, msg);
    if (chan_size < chan->cap) {
      return false;
    } else {
      // sleep in background
      COGO_CQ_PUSH(&chan->cq, cogo_this);
      cogo_this->base_await.sched->base_await_sched.top = NULL;
      return true;
    }
  }
}

int cogo_async_sched_push(cogo_async_sched_t* const sched, cogo_async_t* const co) {
  COGO_ASSERT(sched && co);
  COGO_CQ_PUSH(&sched->q, co);
  return true;  // switch context
}

cogo_async_t* cogo_async_sched_pop(cogo_async_sched_t* const sched) {
  COGO_ASSERT(sched);
  return COGO_CQ_POP(&sched->q);
}

// run until yield, return the next coroutine will be run
cogo_pc_t cogo_async_resume(cogo_async_t* const co) {
#define TOP (co->base_await.top)
  COGO_ASSERT(co);
  if (co->base_await.top) {
    cogo_async_sched_t sched = {
        .base_await_sched = {
            .top = TOP,
        },
        .q = {
            .head = TOP->next,
            .tail = TOP->next,
        },
    };
    if (sched.q.tail) {
      while (sched.q.tail->next) {
        sched.q.tail = sched.q.tail->next;
      }
    }

    // save resume point
    TOP = cogo_async_sched_resume(&sched);
    // save q
    if (TOP) {
      TOP->next = sched.q.head;
    }
  }
  return TOP ? COGO_PC(&TOP->base_await.base_yield.base_pt) : COGO_PC_END;
#undef TOP
}

void cogo_async_run(cogo_async_t* const cogo) {
  COGO_ASSERT(co);
  cogo_async_sched_t sched = {
      .base_await_sched = {
          .top = cogo,
      },
  };
  while (cogo_async_sched_step(&sched)) {
  }

  /* mt
  for (;;) {
    if (resume(&sched)) {
      wait.wake
    } else {
      if (steal_coroutine() == 0) {
        wait.sleep
      }
    }
  }
  */
}
