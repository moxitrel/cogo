#include "cogo/cogo_paral.h"

// Run until yield. Return the yield coroutine.
COGO_T* cogo_paralsched_resume(COGO_SCHED_T* const sched) {
#define TOP        COGO_SCHED_TOP(sched)
#define TOP_STATUS COGO_STATUS(TOP)
#define TOP_FUNC   COGO_OF_FUNC(TOP)
#define TOP_CALLER COGO_OF_CALLER(TOP)
#define TOP_SCHED  COGO_OF_SCHED(TOP)
    COGO_T* top0;
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));

    for (;;) {
        // end | blocked | over
        if (!(top0 = TOP) && !(top0 = TOP = COGO_SCHED_REMOVE(sched))) {
            break;
        }

        COGO_ASSERT(COGO_IS_VALID(TOP));

        // awaited
        if (TOP_STATUS == COGO_STATUS_END) {
            TOP = TOP_CALLER;
            continue;
        }

        TOP_SCHED = sched;
        TOP_FUNC(TOP);
        if (TOP && TOP_STATUS > COGO_STATUS_BEGIN && TOP == top0) {  // yield
            COGO_SCHED_ADD(sched, TOP);
            TOP = COGO_SCHED_REMOVE(sched);
            break;
        }

        // blocked | awaiting (resumed)
    }

    return top0;

#undef TOP_SCHED
#undef TOP_CALLER
#undef TOP_FUNC
#undef TOP_STATUS
#undef TOP
}

// run until yield, return the next coroutine will be run
const COGO_T* cogo_paral_resume(COGO_T* const cogo) {
    COGO_T* top;
    // COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_OF_TOP(cogo));
    if (COGO_OF_NEXT(cogo)) {
        COGO_SCHED_CQ(&sched)->head = COGO_OF_NEXT(cogo);
        COGO_SCHED_CQ(&sched)->tail = COGO_OF_NEXT(cogo);
        while (COGO_OF_NEXT(COGO_SCHED_CQ(&sched)->tail)) {
            COGO_SCHED_CQ(&sched)->tail = COGO_OF_NEXT(COGO_SCHED_CQ(&sched)->tail);
        }
    }

    top = COGO_SCHED_RESUME(&sched);

    COGO_OF_NEXT(cogo) = COGO_SCHED_CQ(&sched)->head;
    COGO_OF_TOP(cogo) = COGO_SCHED_TOP(&sched);
    return top;
}

int cogo_chan_read(COGO_T* const cogo, cogo_chan_t* const chan, cogo_msg_t* const msg_next) {
#define SCHED     COGO_OF_SCHED(cogo)
#define SCHED_TOP COGO_SCHED_TOP(SCHED)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(cogo) && COGO_CHAN_IS_VALID(chan) && msg_next);

    chan_size = chan->size--;
    if (chan_size <= 0) {
        COGO_MQ_ADD(&chan->mq, msg_next);
        // sleep in background
        COGO_CQ_ADD(&chan->cq, cogo);  // append to blocking queue
        SCHED_TOP = 0;                 // remove from scheduler
        return 1;                      // switch context
    } else {
        msg_next->next = COGO_MQ_REMOVE_NONEMPTY(&chan->mq);
        if (chan_size <= chan->capacity) {
            return 0;
        } else {
            // wake up a writer
            return COGO_SCHED_ADD(SCHED, COGO_CQ_REMOVE_NONEMPTY(&chan->cq));
        }
    }

#undef SCHED_TOP
#undef SCHED
}

int cogo_chan_write(COGO_T* const cogo, cogo_chan_t* const chan, cogo_msg_t* const msg) {
#define SCHED     COGO_OF_SCHED(cogo)
#define SCHED_TOP COGO_SCHED_TOP(SCHED)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(cogo) && COGO_CHAN_IS_VALID(chan) && msg);

    chan_size = chan->size++;
    if (chan_size < 0) {
        COGO_MQ_REMOVE_NONEMPTY(&chan->mq)->next = msg;
        // wake up a reader
        return COGO_SCHED_ADD(SCHED, COGO_CQ_REMOVE_NONEMPTY(&chan->cq));
    } else {
        COGO_MQ_ADD(&chan->mq, msg);
        if (chan_size < chan->capacity) {
            return 0;
        } else {
            // sleep in background
            COGO_CQ_ADD(&chan->cq, cogo);
            SCHED_TOP = 0;
            return 1;
        }
    }

#undef SCHED_TOP
#undef SCHED
}

void cogo_paral_run(COGO_T* const cogo) {
    // COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_OF_TOP(cogo));
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

int cogo_paralsched_add(COGO_SCHED_T* const sched, COGO_T* const cogo) {
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched) && COGO_IS_VALID(cogo));
    COGO_CQ_ADD(COGO_SCHED_CQ(sched), cogo);
    return 1;  // switch context
}

COGO_T* cogo_paralsched_remove(COGO_SCHED_T* const sched) {
    COGO_ASSERT(COGO_SCHED_IS_VALID(sched));
    return COGO_CQ_REMOVE(COGO_SCHED_CQ(sched));
}
