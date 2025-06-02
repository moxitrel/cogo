#include <cogo/cogo_async.h>

// Run until yield. Return the yield coroutine.
COGO_T const* cogo_async_sched_resume(COGO_SCHED_T* const sched) {
#define TOP         COGO_SCHED_TOP_OF(sched)
#define TOP_PC      COGO_PC(TOP)
#define TOP_FUNC    COGO_FUNC_OF(TOP)
#define TOP_AWAITER COGO_AWAITER_OF(TOP)
#define TOP_SCHED   COGO_SCHED_OF(TOP)
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));

    for (;;) {
        if (!TOP) {                                         // blocked | end
            if (!(TOP = cogo_async_sched_remove(sched))) {  // over
                goto exit;
            }
        }

        COGO_ASSERT(COGO_IS_VALID(TOP));
        TOP_SCHED = sched;
        TOP_FUNC(TOP);
        if (TOP) {  // not blocked
            switch (TOP_PC) {
                case COGO_PC_END:  // awaited | end
                    TOP = TOP_AWAITER;
                    continue;
                case COGO_PC_BEGIN:  // awaiting
                    continue;
                default:  // yield
                    cogo_async_sched_add(sched, TOP);
                    goto exit;
            }
        }
    }

exit: {
    COGO_T const* top0 = TOP;
    TOP = cogo_async_sched_remove(sched);
    return top0;
}

#undef TOP_SCHED
#undef TOP_AWAITER
#undef TOP_FUNC
#undef TOP_PC
#undef TOP
}

int cogo_chan_read(cogo_async_t* const async, cogo_chan_t* const chan, cogo_msg_t* const msg_next) {
#define SCHED     COGO_SCHED_OF(async)
#define SCHED_TOP COGO_SCHED_TOP_OF(SCHED)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(async) && COGO_CHAN_IS_VALID(chan) && msg_next);

    chan_size = chan->size--;
    if (chan_size <= 0) {
        COGO_MQ_PUSH(&chan->mq, msg_next);
        // sleep in background
        COGO_CQ_PUSH(&chan->cq, async);  // append to blocking queue
        SCHED_TOP = 0;                   // remove from scheduler
        return 1;                        // switch context
    } else {
        msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
        if (chan_size <= chan->capacity) {
            return 0;
        } else {
            // wake up a writer
            return cogo_async_sched_add(SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
        }
    }

#undef SCHED_TOP
#undef SCHED
}

int cogo_chan_write(cogo_async_t* const async, cogo_chan_t* const chan, cogo_msg_t* const msg) {
#define SCHED     (async->await.a.sched)
#define SCHED_TOP (SCHED->await_sched.top)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(async) && COGO_CHAN_IS_VALID(chan) && msg);

    chan_size = chan->size++;
    if (chan_size < 0) {
        COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
        // wake up a reader
        return cogo_async_sched_add(SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
    } else {
        COGO_MQ_PUSH(&chan->mq, msg);
        if (chan_size < chan->capacity) {
            return 0;
        } else {
            // sleep in background
            COGO_CQ_PUSH(&chan->cq, async);
            SCHED_TOP = 0;
            return 1;
        }
    }

#undef SCHED_TOP
#undef SCHED
}

int cogo_async_sched_add(cogo_async_sched_t* const sched, cogo_async_t* const cogo) {
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched) && COGO_IS_VALID(cogo));
    COGO_CQ_PUSH(&sched->cq, cogo);
    return 1;  // switch context
}

cogo_async_t* cogo_async_sched_remove(cogo_async_sched_t* const sched) {
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));
    return COGO_CQ_POP(&sched->cq);
}

// run until yield, return the next coroutine will be run
COGO_T const* cogo_async_resume(COGO_T* const cogo) {
    // COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_T const* top0;
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(cogo));
    if (cogo->next) {
        sched.cq.head = cogo->next;
        sched.cq.tail = cogo->next;
        while (sched.cq.tail->next) {
            sched.cq.tail = sched.cq.tail->next;
        }
    }

    top0 = cogo_async_sched_resume(&sched);

    if (sched.cq.head) {
        cogo->next = sched.cq.head;
    }
    COGO_TOP_OF(cogo) = COGO_SCHED_TOP_OF(&sched);
    return top0;
}

void cogo_async_run(cogo_async_t* const cogo) {
    // COGO_ASSERT(COGO_IS_VALID(cogo));
    cogo_async_sched_t sched = COGO_ASYNC_SCHED_INIT(cogo);
    while (cogo_async_sched_resume(&sched)) {
    }

    /* mt
  for (;;) {
    if (func(&sched)) {
      wait.wake
    } else {
      if (steal_coroutine() == 0) {
        wait.sleep
      }
    }
  }
  */
}
