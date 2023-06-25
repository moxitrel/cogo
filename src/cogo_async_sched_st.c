#include <cogo/cogo_async.h>

int cogo_async_sched_push(cogo_async_sched_t* const sched, cogo_async_t* const co) {
  COGO_ASSERT(sched && co);
  COGO_CQ_PUSH(&sched->q, co);
  return 1;  // switch context
}

cogo_async_t* cogo_async_sched_pop(cogo_async_sched_t* const sched) {
  COGO_ASSERT(sched);
  return COGO_CQ_POP(&sched->q);
}

void cogo_async_run(cogo_async_t* co) {
  COGO_ASSERT(co);
  cogo_async_sched_t sched = {
      .super = {
          .call_top = (cogo_await_t*)(co),
      },
  };
  while (cogo_async_sched_resume(&sched)) {
    // noop
  }
}
