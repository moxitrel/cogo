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

  co->caller = thiz;           // call stack push
  thiz->sched->top = co->top;  // continue from resume point
}

// run until yield
cogo_pc_t cogo_await_resume(cogo_await_t* const co) {
#define TOP (sched.top)
  COGO_ASSERT(co);
  if (COGO_PC(co) != COGO_PC_END) {
    cogo_await_sched_t sched = {
        .top = co->top,  // restore resume point
    };
    for (;;) {
      TOP->sched = &sched;
      TOP->resume(TOP);
      switch (COGO_PC(TOP)) {
        case COGO_PC_END:  // return
          TOP = TOP->caller;
          if (!TOP) {  // end
            goto exit;
          }
          continue;
        case COGO_PC_BEGIN:  // await
          continue;
        default:  // yield
          goto exit;
      }
    }
  exit:
    co->top = TOP;  // save resume point
  }
  return COGO_PC(co);
#undef TOP
}

void cogo_await_run(cogo_await_t* const co) {
  COGO_ASSERT(co);
  while (cogo_await_resume(co) != COGO_PC_END) {
  }
}
