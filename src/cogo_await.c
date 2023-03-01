#include <cogo/cogo_await.h>

cogo_await_t* cogo_sch_step(cogo_await_sch_t* const sch) {
  COGO_ASSERT(sch);
  while (sch->stack_top) {
    sch->stack_top->sch = sch;
    sch->stack_top->func(sch->stack_top);
    if (!sch->stack_top) {
      // blocked
      break;
    }
    switch (co_status(sch->stack_top)) {
      case CO_STATUS_INIT:  // await
        continue;
      case CO_STATUS_FINI:  // return
        sch->stack_top = sch->stack_top->caller;
        continue;
      default:  // yield
        cogo_sch_push(sch, sch->stack_top);
        goto break_while;
    }
  }
break_while:
  return sch->stack_top = cogo_sch_pop(sch);
}
