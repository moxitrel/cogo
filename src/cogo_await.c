#include <cogo/cogo_await.h>

// Should be invoked through CO_AWAIT().
void cogo_await_await(cogo_await_t* const cogo_this, cogo_await_t* const cogo1_base) {
  COGO_ASSERT(cogo_this && cogo_this->sched && cogo1_base);
#ifndef NDEBUG
  // No loop in the call chain.
  for (cogo_await_t const* node = cogo_this; node; node = node->caller) {
    COGO_ASSERT(cogo1_base != node);
  }
#endif
  cogo1_base->caller = cogo_this->sched->top;  // call stack push
  cogo_this->sched->top = cogo1_base->top;     // continue from resume point
}

// Run until CO_YIELD().
cogo_pc_t cogo_await_resume(cogo_await_t* const cogo_this) {
#define TOP (sched.top)
  COGO_ASSERT(cogo_this);
  if (COGO_PC(cogo_this) != COGO_PC_END) {
    cogo_await_sched_t sched = {
        .top = cogo_this->top,  // Restore the resume point.
    };
    for (;;) {
      TOP->sched = &sched;
      COGO_YIELD_V(TOP)->resume(TOP);
      switch (COGO_PC(TOP)) {
        case COGO_PC_END:  // return
          TOP = TOP->caller;
          if (!TOP) {  // end
            goto resume_end;
          }
          continue;
        case COGO_PC_BEGIN:  // await
          continue;
        default:  // yield
          goto resume_end;
      }
    }
  resume_end:
    cogo_this->top = TOP;  // Save the resume point.
  }
  return COGO_PC(cogo_this);
#undef TOP
}
