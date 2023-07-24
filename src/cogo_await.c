#include <cogo/cogo_await.h>

void cogo_await_await(cogo_await_t* const thiz, cogo_await_t* const co) {
  COGO_ASSERT(thiz && thiz->sched && co);

#ifndef NDEBUG
  // no loop in call chain
  for (cogo_await_t const* node = thiz; node; node = node->caller) {
    COGO_ASSERT(co != node);
  }
#endif

  // call stack push
  co->caller = thiz->sched->top;  // ".top == thiz" in normal case
  thiz->sched->top = co->resume ? co->resume : co;
}

// run until yield
co_status_t cogo_await_resume(cogo_await_t* const co) {
  COGO_ASSERT(co);
  if (CO_STATUS(co) != CO_STATUS_END) {
    cogo_await_sched_t sched = {
        .top = co->resume ? co->resume : co,  // resume
    };
    for (;;) {
      sched.top->sched = &sched;
      sched.top->base.func(sched.top);
      switch (CO_STATUS(sched.top)) {
        case CO_STATUS_END:  // return
          sched.top = sched.top->caller;
          if (!sched.top) {  // end
            goto exit;
          }
          continue;
        case CO_STATUS_BEGIN:  // await
          continue;
        default:  // yield
          goto exit;
      }
    }
  exit:
    co->resume = sched.top;  // save resume point
  }
  return CO_STATUS(co);
}

void cogo_await_run(cogo_await_t* const co) {
  COGO_ASSERT(co);
  while (cogo_await_resume(co) != CO_STATUS_END) {
  }
}
