#include "cogo/cogo_paral.h"

// Run until yield. Return the yield coroutine.
COGO_PARAL_T* cogo_paralsched_resume(COGO_PARALSCHED_T* const paralsched) {
#define TOP        COGO_PARALSCHED_TOP(paralsched)
#define TOP_STATUS COGO_STATUS(TOP)
#define TOP_FUNC   COGO_FUNC_OF(TOP)
#define TOP_CALLER COGO_CALLER_OF(TOP)
#define TOP_SCHED  COGO_SCHED_OF(TOP)
    COGO_T* top0;
    COGO_ASSERT(COGO_PARALSCHED_IS_VALID(paralsched));

    for (;;) {
        // end | blocked | over
        if (!(top0 = TOP) && !(top0 = TOP = COGO_PARALSCHED_REMOVE(paralsched))) {
            break;
        }

        COGO_ASSERT(COGO_IS_VALID(TOP));

        // awaited
        if (TOP_STATUS == COGO_STATUS_END) {
            TOP = TOP_CALLER;
            continue;
        }

        TOP_SCHED = paralsched;
        TOP_FUNC(TOP);
        if (TOP && TOP_STATUS > COGO_STATUS_BEGIN && TOP == top0) {  // yield
            COGO_PARALSCHED_ADD(paralsched, TOP);
            TOP = COGO_PARALSCHED_REMOVE(paralsched);
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
const COGO_PARAL_T* cogo_paral_resume(COGO_PARAL_T* const paral) {
    COGO_T* top;
    // COGO_ASSERT(COGO_IS_VALID(cogo));
    COGO_PARALSCHED_T paralsched = COGO_PARALSCHED_INIT(COGO_PARAL_TOP(paral));
    if (COGO_PARAL_NEXT(paral)) {
        COGO_PARALSCHED_CQ(&paralsched)->head = COGO_PARAL_NEXT(paral);
        COGO_PARALSCHED_CQ(&paralsched)->tail = COGO_PARAL_NEXT(paral);
        while (COGO_PARAL_NEXT(COGO_PARALSCHED_CQ(&paralsched)->tail)) {
            COGO_PARALSCHED_CQ(&paralsched)->tail = COGO_PARAL_NEXT(COGO_PARALSCHED_CQ(&paralsched)->tail);
        }
    }

    top = COGO_PARALSCHED_RESUME(&paralsched);

    COGO_PARAL_NEXT(paral) = COGO_PARALSCHED_CQ(&paralsched)->head;
    COGO_PARAL_TOP(paral) = COGO_PARALSCHED_TOP(&paralsched);
    return top;
}

void cogo_paral_run(COGO_PARAL_T* const paral) {
    // COGO_ASSERT(COGO_IS_VALID(paral));
    COGO_SCHED_T sched = COGO_SCHED_INIT(COGO_TOP_OF(paral));
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

int cogo_paralsched_add(COGO_PARALSCHED_T* const paralsched, COGO_PARAL_T* const paral) {
    COGO_ASSERT(COGO_PARALSCHED_IS_VALID(paralsched) && COGO_PARAL_IS_VALID(paral));
    COGO_CQ_ADD(COGO_PARALSCHED_CQ(paralsched), paral);
    return 1;  // switch context
}

COGO_PARAL_T* cogo_paralsched_remove(COGO_PARALSCHED_T* const paralsched) {
    COGO_ASSERT(COGO_PARALSCHED_IS_VALID(paralsched));
    return COGO_CQ_REMOVE(COGO_PARALSCHED_CQ(paralsched));
}

int cogo_chan_read(COGO_T* const cogo, COGO_CHAN_T* const chan, COGO_MSG_T* const msg_next) {
#define SCHED     COGO_SCHED_OF(cogo)
#define SCHED_TOP COGO_SCHED_TOP(SCHED)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(cogo) && COGO_CHAN_IS_VALID(chan) && msg_next);

    chan_size = COGO_CHAN_SIZE(chan)--;
    if (chan_size <= 0) {
        COGO_CHAN_ADD_MSG(chan, msg_next);
        // sleep in background
        COGO_CHAN_ADD_COGO(chan, cogo);  // append to blocking queue
        SCHED_TOP = 0;                   // remove from scheduler
        return 1;                        // switch context
    } else {
        COGO_MSG_NEXT(msg_next) = COGO_CHAN_REMOVE_MSG(chan);
        if (chan_size <= COGO_CHAN_MAXSIZE(chan)) {
            return 0;
        } else {
            // wake up a writer
            return COGO_SCHED_ADD(SCHED, COGO_CHAN_REMOVE_COGO_NONEMPTY(chan));
        }
    }

#undef SCHED_TOP
#undef SCHED
}

int cogo_chan_write(COGO_T* const cogo, COGO_CHAN_T* const chan, COGO_MSG_T* const msg) {
#define SCHED     COGO_SCHED_OF(cogo)
#define SCHED_TOP COGO_SCHED_TOP(SCHED)
    long chan_size;
    COGO_ASSERT(COGO_IS_VALID(cogo) && COGO_CHAN_IS_VALID(chan) && msg);

    chan_size = COGO_CHAN_SIZE(chan)++;
    if (chan_size < 0) {
        COGO_MSG_NEXT(COGO_CHAN_REMOVE_MSG_NONEMPTY(chan)) = msg;
        // wake up a reader
        return COGO_SCHED_ADD(SCHED, COGO_CHAN_REMOVE_COGO_NONEMPTY(chan));
    } else {
        COGO_CHAN_ADD_MSG(chan, msg);
        if (chan_size < COGO_CHAN_MAXSIZE(chan)) {
            return 0;
        } else {
            // sleep in background
            COGO_CHAN_ADD_COGO(chan, cogo);
            SCHED_TOP = 0;
            return 1;
        }
    }

#undef SCHED_TOP
#undef SCHED
}
