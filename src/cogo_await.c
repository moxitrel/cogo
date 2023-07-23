#include <cogo/cogo_await.h>

// return the next coroutine to be run
// should be invoked through CO_WAIT()
cogo_await_t* cogo_await_await(cogo_await_t* const thiz, cogo_await_t* const callee) {
  COGO_ASSERT(thiz && callee);

#ifndef NDEBUG
  // no loop in call chain
  for (cogo_await_t const* co = thiz; co; co = co->caller) {
    COGO_ASSERT(callee != co);
  }
#endif

  // call stack push
  callee->caller = thiz;  // WARN: stack damaged if cogo_await_await() is invoked directly more than once
  // cogo_async: thiz->sched->top = callee->top;
  // cogo_await: thiz->top = callee->top;
  return callee->top ? /*resume*/ callee->top : /*begin*/ callee;
}

// run until yield
co_status_t cogo_await_resume(cogo_await_t* const co) {
  COGO_ASSERT(co);
  if (CO_STATUS(co) != CO_STATUS_END) {
    cogo_await_t* top = co->top ? /*resume*/ co->top : /*begin*/ co;
    for (;;) {
      top->top = top;
      top->super.func(top);
      top = top->top;
      switch (CO_STATUS(top)) {
        case CO_STATUS_END:  // return
          top = top->caller;
          if (!top) {  // end
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
    co->top = top;  // save resume point
  }
  return CO_STATUS(co);
}

void cogo_await_run(cogo_await_t* const co) {
  COGO_ASSERT(co);
  while (cogo_await_resume(co) != CO_STATUS_END) {
  }
}
