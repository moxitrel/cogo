#include <cogo/cogo_co.h>

// implement cogo_await_sched_push()
int cogo_await_sched_push(cogo_await_sched_t* const sched, cogo_await_t* const co) {
  COGO_ASSERT(sched && co);
  (COGO_QUEUE_PUSH(cogo_co_t)(&((cogo_co_sched_t*)sched)->q, (cogo_co_t*)co));
  return 1;  // switch context
}

// implement cogo_await_sched_pop()
cogo_await_t* cogo_await_sched_pop(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched);
  return &COGO_QUEUE_POP(cogo_co_t)(&((cogo_co_sched_t*)sched)->q)->super;
}

void co_run(void* const co) {
  cogo_co_sched_t sched = {
      .super = {
          .stack_top = (cogo_await_t*)co,
      },
  };
  while (cogo_await_sched_step(&sched.super)) {
    // noop
  }
}
