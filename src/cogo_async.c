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
        if (!TOP) {                                   // blocked | end
            if (!(TOP = COGO_SCHED_REMOVE(sched))) {  // over
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
                    COGO_SCHED_ADD(sched, TOP);
                    goto exit;
            }
        }
    }

exit: {
    COGO_T const* y = TOP;
    TOP = COGO_SCHED_REMOVE(sched);
    return y;
}

#undef TOP_SCHED
#undef TOP_AWAITER
#undef TOP_FUNC
#undef TOP_PC
#undef TOP
}

int cogo_chan_read(COGO_T* const cogo, cogo_chan_t* const chan, cogo_msg_t* const msg_next) {
#define SCHED     COGO_SCHED_OF(cogo)
#define SCHED_TOP COGO_SCHED_TOP_OF(SCHED)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(cogo) && COGO_CHAN_IS_VALID(chan) && msg_next);

    chan_size = chan->size--;
    if (chan_size <= 0) {
        COGO_MQ_PUSH(&chan->mq, msg_next);
        // sleep in background
        COGO_CQ_PUSH(&chan->cq, cogo);  // append to blocking queue
        SCHED_TOP = 0;                  // remove from scheduler
        return 1;                       // switch context
    } else {
        msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
        if (chan_size <= chan->capacity) {
            return 0;
        } else {
            // wake up a writer
            return COGO_SCHED_ADD(SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
        }
    }

#undef SCHED_TOP
#undef SCHED
}

int cogo_chan_write(COGO_T* const cogo, cogo_chan_t* const chan, cogo_msg_t* const msg) {
#define SCHED     COGO_SCHED_OF(cogo)
#define SCHED_TOP COGO_SCHED_TOP_OF(SCHED)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(cogo) && COGO_CHAN_IS_VALID(chan) && msg);

    chan_size = chan->size++;
    if (chan_size < 0) {
        COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
        // wake up a reader
        return COGO_SCHED_ADD(SCHED, COGO_CQ_POP_NONEMPTY(&chan->cq));
    } else {
        COGO_MQ_PUSH(&chan->mq, msg);
        if (chan_size < chan->capacity) {
            return 0;
        } else {
            // sleep in background
            COGO_CQ_PUSH(&chan->cq, cogo);
            SCHED_TOP = 0;
            return 1;
        }
    }

#undef SCHED_TOP
#undef SCHED
}

// run until yield, return the next coroutine will be run
COGO_T const* cogo_async_resume(COGO_T* const cogo) {
    // COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_T const* y;
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(cogo));
    if (cogo->next) {
        sched.cq.head = cogo->next;
        sched.cq.tail = cogo->next;
        while (sched.cq.tail->next) {
            sched.cq.tail = sched.cq.tail->next;
        }
    }

    y = COGO_SCHED_RESUME(&sched);

    if (sched.cq.head) {
        cogo->next = sched.cq.head;
    }
    COGO_TOP_OF(cogo) = COGO_SCHED_TOP_OF(&sched);
    return y;
}

void cogo_async_run(COGO_T* const cogo) {
    // COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(cogo));
    while (COGO_SCHED_RESUME(&sched)) {
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

int cogo_async_sched_add(COGO_SCHED_T* const sched, COGO_T* const cogo) {
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched) && COGO_IS_VALID(cogo));
    COGO_CQ_PUSH(&sched->cq, cogo);
    return 1;  // switch context
}

COGO_T* cogo_async_sched_remove(COGO_SCHED_T* const sched) {
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));
    return COGO_CQ_POP(&sched->cq);
}
