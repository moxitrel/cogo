#include <cogo/cogo_async.h>
#include <limits.h>

// Run until yield. Return the next coroutine to be run.
static cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched) {
#define TOP         (sched->base_await_sched.top)
#define TOP_SCHED   (COGO_AWAIT_OF(TOP)->anon.sched)
#define TOP_AWAITER (COGO_AWAIT_OF(TOP)->awaiter)
#define TOP_FUNC    (COGO_AWAIT_OF(TOP)->func)
    COGO_ASSERT(sched);

    for (;;) {
        COGO_ASSERT(cogo_async_is_valid(TOP));
        TOP_SCHED = sched;
        TOP_FUNC(TOP);
        if (!TOP) {  // blocking
            goto exit;
        } else {
            switch (COGO_PC(TOP)) {
                case COGO_PC_END:
                    if (!(TOP = TOP_AWAITER)) {  // end
                        goto exit;
                    }
                    continue;        // awaited
                case COGO_PC_BEGIN:  // awaiting
                    continue;
                default:  // yielding
                    cogo_async_sched_add(sched, TOP);
                    goto exit;
            }
        }
    }

exit:
    return TOP = cogo_async_sched_remove(sched);
#undef TOP_FUNC
#undef TOP_AWAITER
#undef TOP_SCHED
#undef TOP
}

int cogo_chan_read(cogo_async_t* const COGO_THIS, cogo_chan_t* const chan, cogo_msg_t* const msg_next) {
#define SCHED     (COGO_THIS->base_await.anon.sched)
#define SCHED_TOP (SCHED->base_await_sched.top)
    ptrdiff_t chan_size;
    COGO_ASSERT(cogo_async_is_valid(COGO_THIS) && chan && chan->capacity >= 0 && chan->size > PTRDIFF_MIN && msg_next);

    chan_size = chan->size--;
    if (chan_size <= 0) {
        COGO_MQ_PUSH(&chan->mq, msg_next);
        // sleep in background
        COGO_CQ_PUSH(&chan->cq, COGO_THIS);  // append to blocking queue
        SCHED_TOP = NULL;                    // remove from scheduler
        return 1;                            // switch context
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

int cogo_chan_write(cogo_async_t* const COGO_THIS, cogo_chan_t* const chan, cogo_msg_t* const msg) {
#define SCHED     (COGO_THIS->base_await.anon.sched)
#define SCHED_TOP (SCHED->base_await_sched.top)
    ptrdiff_t chan_size;

    COGO_ASSERT(cogo_async_is_valid(COGO_THIS) && chan && chan->capacity >= 0 && chan->size < PTRDIFF_MAX && msg);
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
            COGO_CQ_PUSH(&chan->cq, COGO_THIS);
            SCHED_TOP = NULL;
            return 1;
        }
    }

#undef SCHED_TOP
#undef SCHED
}

int cogo_async_sched_add(cogo_async_sched_t* const sched, cogo_async_t* const cogo) {
    COGO_ASSERT(sched && cogo_async_is_valid(cogo));
    COGO_CQ_PUSH(&sched->q, cogo);
    return 1;  // switch context
}

cogo_async_t* cogo_async_sched_remove(cogo_async_sched_t* const sched) {
    COGO_ASSERT(sched);
    return COGO_CQ_POP(&sched->q);
}

// run until yield, return the next coroutine will be run
cogo_pc_t cogo_async_resume(cogo_async_t* const cogo) {
#define TOP (COGO_AWAIT_OF(cogo)->anon.top)
    COGO_ASSERT(cogo_async_is_valid(cogo));
    if (TOP) {
        cogo_async_sched_t sched = COGO_ASYNC_SCHED_INIT(TOP);
        COGO_CQ_PUSH(&sched.q, TOP->next);
        if (!COGO_CQ_IS_EMPTY(&sched.q)) {
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
    COGO_ASSERT(cogo_async_is_valid(cogo));
    {
        cogo_async_sched_t sched = COGO_ASYNC_SCHED_INIT(cogo);
        while (cogo_async_sched_resume(&sched)) {
        }
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
