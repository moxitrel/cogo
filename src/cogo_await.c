#include <cogo/cogo_await.h>

// Should be invoked through CO_AWAIT()
void cogo_await_await(cogo_await_t* const cogo_this, cogo_await_t* const cogo) {
  COGO_ASSERT(cogo_this && cogo_this->sched && cogo);
#ifndef NDEBUG
  // No loop in the call chain.
  for (cogo_await_t const* node = cogo_this; node; node = node->caller) {
    COGO_ASSERT(cogo != node);
  }
#endif
  cogo->caller = cogo_this;           // call stack push
  cogo_this->sched->top = cogo->top;  // continue from resume point
}

// run until yield
cogo_pc_t cogo_await_resume(cogo_await_t* const cogo) {
#define TOP (sched.top)
  COGO_ASSERT(cogo);
  if (COGO_PC(&cogo->base_yield.base_pt) != COGO_PC_END) {
    cogo_await_sched_t sched = {
        .top = cogo->top,  // Restore the resume point.
    };
    for (;;) {
      TOP->sched = &sched;
      TOP->base_yield.resume(TOP);
      switch (COGO_PC(&TOP->base_yield.base_pt)) {
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
    cogo->top = TOP;  // Save the resume point.
  }
  return COGO_PC(&cogo->base_yield.base_pt);
#undef TOP
}
