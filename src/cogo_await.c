#include <cogo/cogo_await.h>

// should be invoked through CO_AWAIT()
void cogo_await_await(cogo_await_t* const thiz, cogo_await_t* const co) {
  COGO_ASSERT(thiz && thiz->sched && co);

#ifndef NDEBUG
  // no loop in call chain
  for (cogo_await_t const* node = thiz; node; node = node->caller) {
    COGO_ASSERT(co != node);
  }
#endif

  // call stack push
  co->caller = thiz;
  // continue from resume point
  thiz->sched->top = co->top;
}

// run until yield
co_status_t cogo_await_resume(cogo_await_t* const co) {
#define TOP (sched.top)
  COGO_ASSERT(co);
  if (CO_STATUS(co) != CO_STATUS_END) {
    cogo_await_sched_t sched = {
        .top = co->top,  // resume or begin
    };
    for (;;) {
      TOP->sched = &sched;
      ((cogo_yield_t*)TOP)->resume(TOP);
      switch (CO_STATUS(TOP)) {
        case CO_STATUS_END:  // return
          TOP = TOP->caller;
          if (!TOP) {  // end
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
    co->top = TOP;  // save resume point
  }
  return CO_STATUS(co);
#undef TOP
}

void cogo_await_run(cogo_await_t* const co) {
  COGO_ASSERT(co);
  while (cogo_await_resume(co) != CO_STATUS_END) {
  }
}
