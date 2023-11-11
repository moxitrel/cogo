#include <cogo/cogo_async.h>
#include <limits.h>
#include <stdbool.h>

// cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched) {
// #define TOP (sched->base.top)
//   COGO_ASSERT(sched && TOP);
//   for (;;) {
//     TOP->sched = &sched->base;
//     TOP->base.resume(TOP);
//     if (!TOP) {  // blocked
//       TOP = (cogo_await_t*)cogo_async_sched_pop(sched);
//       if (!TOP) {  // no more active coroutines
//         goto exit;
//       }
//       continue;
//     }
//     switch (CO_STATUS(TOP)) {
//       case CO_STATUS_END:  // return
//         TOP = TOP->caller;
//         if (!TOP) {  // end
//           goto exit_next;
//         }
//         continue;
//       case CO_STATUS_BEGIN:  // await
//         continue;
//       default:  // yield
//         cogo_async_sched_push(sched, (cogo_async_t*)TOP);
//         goto exit_next;
//     }
//   }
// exit_next:
//   TOP = &cogo_async_sched_pop(sched)->base;
// exit:
//   return (cogo_async_t*)TOP;
// #undef TOP
// }

bool cogo_chan_read(cogo_async_t* const thiz, co_chan_t* const chan, co_message_t* const msg_next) {
  COGO_ASSERT(thiz && chan && chan->cap >= 0 && chan->size > PTRDIFF_MIN && msg_next);
  ptrdiff_t const chan_size = chan->size--;
  if (chan_size <= 0) {
    COGO_MQ_PUSH(&chan->mq, msg_next);
    // sleep in background
    COGO_CQ_PUSH(&chan->cq, thiz);  // append to blocking queue
    thiz->base.sched->top = NULL;   // remove from scheduler
    return true;                    // switch context
  } else {
    msg_next->next = COGO_MQ_POP_NONEMPTY(&chan->mq);
    if (chan_size <= chan->cap) {
      return false;
    } else {
      // wake up a writer
      return cogo_async_sched_push((cogo_async_sched_t*)thiz->base.sched, COGO_CQ_POP_NONEMPTY(&chan->cq));
    }
  }
}

bool cogo_chan_write(cogo_async_t* const thiz, co_chan_t* const chan, co_message_t* const msg) {
  COGO_ASSERT(thiz && chan && chan->cap >= 0 && chan->size < PTRDIFF_MAX && msg);
  ptrdiff_t const chan_size = chan->size++;
  if (chan_size < 0) {
    COGO_MQ_POP_NONEMPTY(&chan->mq)->next = msg;
    // wake up a reader
    return cogo_async_sched_push((cogo_async_sched_t*)thiz->base.sched, COGO_CQ_POP_NONEMPTY(&chan->cq));
  } else {
    COGO_MQ_PUSH(&chan->mq, msg);
    if (chan_size < chan->cap) {
      return false;
    } else {
      // sleep in background
      COGO_CQ_PUSH(&chan->cq, thiz);
      thiz->base.sched->top = NULL;
      return true;
    }
  }
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

// run until yield, return the next coroutine will be run
co_status_t cogo_async_resume(cogo_async_t* const co) {
#define TOP (sched.base.top)
  COGO_ASSERT(co);
  if (CO_STATUS(co) != CO_STATUS_END) {
    cogo_async_sched_t sched = {
        .base = {
            .top = co->base.top ? co->base.top : &co->base,
        },
    };
    for (;;) {
      TOP->sched = &sched.base;
      TOP->base.resume(TOP);
      if (!TOP) {  // blocked
        TOP = (cogo_await_t*)cogo_async_sched_pop(&sched);
        if (!TOP) {  // no more active coroutines
          goto exit;
        }
        continue;
      }
      switch (CO_STATUS(TOP)) {
        case CO_STATUS_END:  // return
          TOP = TOP->caller;
          if (!TOP) {  // end
            goto exit_next;
          }
          continue;
        case CO_STATUS_BEGIN:  // await
          continue;
        default:  // yield
          cogo_async_sched_push(&sched, (cogo_async_t*)TOP);
          goto exit_next;
      }
    }
  exit_next:
    TOP = &cogo_async_sched_pop(&sched)->base;
  exit:
    // save resume point
    co->base.top = TOP;
    // FIXME: no support for multi-coroutine (sched.q is dropped because sched is a temporary variable)
    COGO_ASSERT(COGO_CQ_IS_EMPTY(&sched.q));
  }
  return CO_STATUS(co);
#undef TOP
}

void cogo_async_run(cogo_async_t* const co) {
  COGO_ASSERT(co);
  while (cogo_async_resume(co) != CO_STATUS_END) {
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
