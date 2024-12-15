#include <cogo/cogo_async.h>
#include <limits.h>

enum {
  cogo_state_yield,
  cogo_state_end,
  cogo_state_blocked,
};

static cogo_async_t* cogo_async_sched_step(cogo_async_sched_t* const sched) {
#define TOP        (sched->base_await_sched.top)
#define TOP_SCHED  (COGO_AWAIT_V(TOP)->sched)
#define TOP_CALLER (COGO_AWAIT_V(TOP)->caller)
#define TOP_RESUME (COGO_YIELD_V(TOP)->resume)

  COGO_ASSERT(sched && TOP && TOP_RESUME);
  TOP_SCHED = sched;
  TOP_RESUME(TOP);
  if (!(/*blocked*/ !TOP || /*end*/ (COGO_PC(TOP) == COGO_PC_END && !(TOP = TOP_CALLER)))) {
    cogo_async_sched_add(sched, TOP);
  }
  return TOP = cogo_async_sched_remove(sched);

#undef TOP_RESUME
#undef TOP_CALLER
#undef TOP_SCHED
#undef TOP
}

// Run until yield. Return the next coroutine to be run.
static cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched) {
#define TOP        (sched->base_await_sched.top)
#define TOP_SCHED  (TOP->base_await.sched)
#define TOP_CALLER (TOP->base_await.caller)
#define TOP_RESUME (TOP->base_await.base_yield.resume)

  COGO_ASSERT(sched);
  for (;;) {
    COGO_ASSERT(TOP && TOP_RESUME);
    TOP_SCHED = sched;
    TOP_RESUME(TOP);
    if (!TOP) {  // blocked
      goto on_blocked;
    } else {
      switch (COGO_PC(TOP)) {
        case COGO_PC_END:             // return
          if (!(TOP = TOP_CALLER)) {  // end
            goto on_end;
          }
          continue;
        case COGO_PC_BEGIN:  // await
          continue;
        default:  // yield, async
          cogo_async_sched_add(sched, TOP);
          TOP = cogo_async_sched_remove(sched);
          goto exit;
      }
    }
  on_blocked:
  on_end:
    if (!(TOP = cogo_async_sched_remove(sched))) {  // no more active coroutines
      goto exit;
    }
  }
exit:
  return TOP;

#undef TOP_RESUME
#undef TOP_CALLER
#undef TOP_SCHED
#undef TOP
}

int cogo_chan_read(cogo_async_t* const cogo_this, cogo_chan_t* const chan, cogo_msg_t* const msg_next) {
  COGO_ASSERT(cogo_this && chan && chan->maxsize >= 0 && chan->size > PTRDIFF_MIN && msg_next);
  ptrdiff_t const chan_size = chan->size--;
  if (chan_size <= 0) {
    COGO_MQ_PUSH(&chan->mq, msg_next);
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, cogo_this);                        // append to blocking queue
    cogo_this->base_await.sched->base_await_sched.top = NULL;  // remove from scheduler
    return 1;                                                  // switch context
  } else {
    msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
    if (chan_size <= chan->maxsize) {
      return 0;
    } else {
      // wake up a writer
      return cogo_async_sched_add(cogo_this->base_await.sched, COGO_CQ_POP_NONEMPTY(&chan->cq));
    }
  }
}

int cogo_chan_write(cogo_async_t* const cogo_this, cogo_chan_t* const chan, cogo_msg_t* const msg) {
  COGO_ASSERT(cogo_this && chan && chan->maxsize >= 0 && chan->size < PTRDIFF_MAX && msg);
  ptrdiff_t const chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
    // wake up a reader
    return cogo_async_sched_add(cogo_this->base_await.sched, COGO_CQ_POP_NONEMPTY(&chan->cq));
  } else {
    COGO_MQ_PUSH(&chan->mq, msg);
    if (chan_size < chan->maxsize) {
      return 0;
    } else {
      // sleep in background
      COGO_CQ_PUSH(&chan->cq, cogo_this);
      cogo_this->base_await.sched->base_await_sched.top = NULL;
      return 1;
    }
  }
}

int cogo_async_sched_add(cogo_async_sched_t* const sched, cogo_async_t* const cogo) {
  COGO_ASSERT(sched && cogo);
  COGO_CQ_PUSH(&sched->q, cogo);
  return 1;  // switch context
}

cogo_async_t* cogo_async_sched_remove(cogo_async_sched_t* const sched) {
  COGO_ASSERT(sched);
  return COGO_CQ_POP(&sched->q);
}

// run until yield, return the next coroutine will be run
cogo_pc_t cogo_async_resume(cogo_async_t* const cogo) {
#define TOP (cogo->base_await.top)
  COGO_ASSERT(cogo);
  if (TOP) {
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
  return TOP ? COGO_PC(TOP) : COGO_PC_END;
#undef TOP
}

void cogo_async_run(cogo_async_t* const cogo) {
  COGO_ASSERT(cogo);
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
