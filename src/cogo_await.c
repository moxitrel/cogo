#include <cogo/cogo_await.h>

// Should be invoked through CO_AWAIT().
void cogo_await_await(cogo_await_t* const cogo_this, cogo_await_t* const cogo1) {
#define TOP (cogo_this->sched->top)

#ifdef COGO_DEBUG
  cogo_await_t const* node;
  // No loop in the call chain.
  for (node = cogo_this; node; node = node->caller) {
    COGO_ASSERT(cogo1 != node);
  }
#endif
  COGO_ASSERT(cogo_this && cogo_this->sched && cogo1);

  cogo1->caller = TOP;  // call stack push
  TOP = cogo1->top;     // continue from resume point

#undef TOP
}

// Run until CO_YIELD().
cogo_pc_t cogo_await_resume(cogo_await_t* const cogo_this) {
#define TOP        (sched.top)
#define TOP_SCHED  (COGO_AWAIT_V(TOP)->sched)
#define TOP_CALLER (COGO_AWAIT_V(TOP)->caller)
#define TOP_FUNC   (COGO_YIELD_V(TOP)->func)
  COGO_ASSERT(cogo_this);

  if (COGO_PC(cogo_this) != COGO_PC_END) {
    cogo_await_sched_t sched = COGO_AWAIT_SCHED_INIT(cogo_this->top);  // Restore the resume point.
    for (;;) {
      COGO_ASSERT(TOP && TOP_FUNC);
      TOP_SCHED = &sched;
      TOP_FUNC(TOP);
      switch (COGO_PC(TOP)) {
        case COGO_PC_END:
          if (!(TOP = TOP_CALLER)) {  // end
            goto exit;
          }
          continue;          // awaited
        case COGO_PC_BEGIN:  // awaiting
          continue;
        default:  // yielding
          goto exit;
      }
    }
  exit:
    cogo_this->top = TOP;  // Save the resume point.
  }

  return COGO_PC(cogo_this);
#undef TOP_FUNC
#undef TOP_CALLER
#undef TOP_SCHED
#undef TOP
}

void cogo_await_run(cogo_await_t* const cogo_this) {
  COGO_ASSERT(cogo_this);
  while (cogo_await_resume(cogo_this) != COGO_PC_END) {
  }
}
