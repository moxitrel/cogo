#include <cogo/cogo_await.h>

#ifdef __GNUC__
#define COGO_FALLTHROUGH __attribute__((fallthrough))
#else
#define COGO_FALLTHROUGH /* fallthrough */
#endif

void cogo_await_call_check(cogo_await_t* const co_this, cogo_await_t* const callee) {
  COGO_ASSERT(co_this && co_this->sched && callee);
  // check no loop in call chain
  for (cogo_await_t* co = co_this; co; co = co->caller) {
    COGO_ASSERT(callee != co);
  }

  cogo_await_t* callee_root = callee;
  while (callee_root->caller) {
    callee_root = callee_root->caller;
  }

  // call stack push (resolve co_this->sched->stack_top != co_this)
  callee->caller = co_this->sched->stack_top;
  //callee->sched = co_this->sched->stack_top->sched;
  co_this->sched->stack_top = callee;
}

// run until yield
cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched && sched->stack_top);
  for (;;) {
    sched->stack_top->sched = sched;
    sched->stack_top->func(sched->stack_top);
    switch (cogo_status(sched->stack_top)) {
      case CO_STATUS_FINI:  // return
        if (!cogo_await_return(sched->stack_top)) {
          // return from root
          goto exit;
        }
        COGO_FALLTHROUGH;
      case CO_STATUS_INIT:  // await
        continue;
      default:  // yield
        goto exit;
    }
  }
exit:
  return sched->stack_top;
}

void cogo_await_run(cogo_await_t* const co) {
  cogo_await_sched_t sched = {
      .stack_top = co,
  };
  while (CO_SCHED_STEP(&sched)) {
    // noop
  }
}
