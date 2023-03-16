#include <cogo/cogo_await.h>

#ifdef __GNUC__
#define COGO_FALLTHROUGH __attribute__((fallthrough))
#else
#define COGO_FALLTHROUGH /* fallthrough */
#endif

void cogo_await_call(cogo_await_t* const co_this, cogo_await_t* const callee) {
  COGO_ASSERT(co_this && co_this->sched && callee);

  // no loop in call chain
  for (cogo_await_t* co = co_this; co; co = co->caller) {
    COGO_ASSERT(callee != co);
  }

  cogo_await_t* callee_root = callee;
  while (callee_root->caller) {
    callee_root = callee_root->caller;
  }

  // call stack push (resolve co_this->sched->call_top != co_this)
  callee_root->caller = co_this->sched->call_top;
  //callee->sched = co_this->sched->call_top->sched;
  co_this->sched->call_top = callee;
}

// run until yield
cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* const sched) {
#define CALL_TOP sched->call_top
  COGO_ASSERT(sched && CALL_TOP);

  for (;;) {
    CALL_TOP->sched = sched;
    CALL_TOP->func(CALL_TOP);
    switch (co_status(&CALL_TOP->super)) {
      case COGO_STATUS_END:  // return
        if (!(CALL_TOP = CALL_TOP->caller)) {
          // return from root
          goto exit;
        }
        COGO_FALLTHROUGH;
      case COGO_STATUS_BEGIN:  // await
        continue;
      default:  // yield
        goto exit;
    }
  }
exit:
  return CALL_TOP;

#undef CALL_TOP
}

cogo_await_t* cogo_await_resume(cogo_await_t* const co) {
  COGO_ASSERT(co);
  cogo_await_sched_t sched = {.call_top = co};
  return cogo_await_sched_step(&sched);
}

void cogo_await_run(cogo_await_t* const co_main) {
  COGO_ASSERT(co_main);
  cogo_await_sched_t sched = {.call_top = co_main};
  while (cogo_await_sched_step(&sched)) {
    // noop
  }
}
