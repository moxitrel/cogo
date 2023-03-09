#include <cogo/cogo_co.h>

int cogo_co_sched_push(cogo_co_sched_t* const sched, cogo_co_t* const co) {
  COGO_ASSERT(sched && co);
  COGO_CQ_PUSH(&sched->q, co);
  return 1;  // switch context
}

cogo_co_t* cogo_co_sched_pop(cogo_co_sched_t* const sched) {
  COGO_ASSERT(sched);
  return COGO_CQ_POP(&sched->q);
}

void cogo_co_sched_run(cogo_co_sched_t* const sched) {
  COGO_ASSERT(sched);
  while (CO_SCHED_STEP(sched)) {
    // noop
  }
}
