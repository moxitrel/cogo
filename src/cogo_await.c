#include <cogo/cogo_await.h>

void cogo_await_call(cogo_await_t* const co_this, cogo_await_t* const callee) {
  COGO_ASSERT(co_this && co_this->sched && callee);

#ifndef NDEBUG
  // no loop in call chain
  for (cogo_await_t* co = co_this; co; co = co->caller) {
    COGO_ASSERT(callee != co);
  }
#endif

  cogo_await_t* callee_bottom = callee;
#ifndef COGO_NO_RESUME
  while (callee_bottom->caller) {
    callee_bottom = callee_bottom->caller;
  }
#else
  COGO_ASSERT(!callee_bottom->caller);
#endif

  // call stack push (resolved co_this->sched->call_top != co_this)
  callee_bottom->caller = co_this->sched->call_top;
  co_this->sched->call_top = callee;
}

// run until yield
// NOTE: .entry is damaged after call
static cogo_await_t* cogo_await_sched_resume(cogo_await_sched_t* const sched) {
#define CALL_TOP (sched->call_top)
  COGO_ASSERT(sched && CALL_TOP);

  for (;;) {
    CALL_TOP->sched = sched;
    CALL_TOP->func(CALL_TOP);
    switch (co_status((cogo_yield_t*)CALL_TOP)) {
      case CO_STATUS_END:  // return
        if (!(CALL_TOP = CALL_TOP->caller)) {
          // return from root
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
  return CALL_TOP;

#undef CALL_TOP
}

#ifndef COGO_NO_RESUME
cogo_await_t* cogo_await_resume(cogo_await_t* const co) {
  COGO_ASSERT(co);
  cogo_await_sched_t sched = {
      .call_top = co->entry ? co->entry : co,
  };
  // repaire .entry
  return co->entry = cogo_await_sched_resume(&sched);
}
#endif

void cogo_await_run(cogo_await_t* const co) {
  COGO_ASSERT(co);
  cogo_await_sched_t sched = {
      .call_top = co->entry ? co->entry : co,
  };
  while (cogo_await_sched_resume(&sched)) {
    // noop
  }
}
