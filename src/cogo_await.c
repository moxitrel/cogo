#include <cogo/cogo_await.h>

cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched);
  while (sched->stack_top) {
    sched->stack_top->sched = sched;
    sched->stack_top->func(sched->stack_top);
    if (!sched->stack_top) {
      // blocked
      break;
    }
    switch (co_status(sched->stack_top)) {
      case CO_STATUS_INIT:  // await
        continue;
      case CO_STATUS_FINI:  // return
        sched->stack_top = sched->stack_top->caller;
        continue;
      default:  // yield
        cogo_await_sched_push(sched, sched->stack_top);
        goto break_while;
    }
  }
break_while:
  return sched->stack_top = cogo_await_sched_pop(sched);
}
