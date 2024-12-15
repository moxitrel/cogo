#include <cogo/cogo_await.h>

// Should be invoked through CO_AWAIT().
void cogo_await_await(cogo_await_t* const cogo_this, cogo_await_t* const cogo1) {
  COGO_ASSERT(cogo_this && cogo_this->sched && cogo1);
#ifdef COGO_DEBUG
  // No loop in the call chain.
  for (cogo_await_t const* node = cogo_this; node; node = node->caller) {
    COGO_ASSERT(cogo1 != node);
  }
#endif
  cogo1->caller = cogo_this->sched->top;  // call stack push
  cogo_this->sched->top = cogo1->top;     // continue from resume point
}

// Run until CO_YIELD().
cogo_pc_t cogo_await_resume(cogo_await_t* const cogo_this) {
#define TOP        (sched.top)
#define TOP_SCHED  (COGO_AWAIT_V(TOP)->sched)
#define TOP_CALLER (COGO_AWAIT_V(TOP)->caller)
#define TOP_RESUME (COGO_YIELD_V(TOP)->resume)
  COGO_ASSERT(cogo_this);

  if (COGO_PC(cogo_this) != COGO_PC_END) {
    cogo_await_sched_t sched = {
        .top = cogo_this->top,  // Restore the resume point.
    };
    for (;;) {
      COGO_ASSERT(TOP && TOP_RESUME);
      TOP_SCHED = &sched;
      TOP_RESUME(TOP);
      switch (COGO_PC(TOP)) {
        case COGO_PC_END:
          if (!(TOP = TOP_CALLER)) {
            goto on_awaited;
          }
          continue;
        case COGO_PC_BEGIN:  // await
          continue;
        default:  // yield
          goto on_yield;
      }
    }
  on_awaited:
  on_yield:
    cogo_this->top = TOP;  // Save the resume point.
  }

  return COGO_PC(cogo_this);
#undef TOP_RESUME
#undef TOP_CALLER
#undef TOP_SCHED
#undef TOP
}

void cogo_await_run(cogo_await_t* const cogo_this) {
  COGO_ASSERT(cogo_this);
  while (cogo_await_resume(cogo_this) != COGO_PC_END) {
  }
}
