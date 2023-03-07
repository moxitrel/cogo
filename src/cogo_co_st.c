#include <cogo/cogo_co.h>

int cogo_co_sched_push(cogo_co_sched_t* sched, cogo_co_t* co) {
  COGO_ASSERT(sched && co);
  (COGO_QUEUE_PUSH(cogo_co_t)(&sched->q, co));
  return 1;  // switch context
}

cogo_co_t* cogo_co_sched_pop(cogo_co_sched_t* sched) {
  COGO_ASSERT(sched);
  return COGO_QUEUE_POP(cogo_co_t)(&sched->q);
}

void cogo_co_run(cogo_co_t* const co) {
  cogo_co_sched_t sched = {
      .super = {
          .stack_top = (cogo_await_t*)co,
      },
  };
  while (CO_SCHED_STEP(&sched)) {
    // noop
  }
}
