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

#ifdef __GNUC__
#define COGO_FALLTHROUGH __attribute__((fallthrough))
#else
#define COGO_FALLTHROUGH /* [[fallthrough]] */
#endif

// run until yield
cogo_await_t* cogo_co_sched_step(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched && sched->stack_top);
  for (;;) {
    sched->stack_top->sched = sched;
    sched->stack_top->func(sched->stack_top);
    if (!sched->stack_top) {
      // blocked
      if (!(sched->stack_top = (cogo_await_t*)cogo_co_sched_pop((cogo_co_sched_t*)sched))) {
        // no more active coroutines
        goto exit;
      }
      continue;
    }
    switch (co_status(sched->stack_top)) {
      case CO_STATUS_FINI:  // return
        if (!cogo_await_return(sched->stack_top)) {
          // return from root
          goto exit_next;
        }
        COGO_FALLTHROUGH;
      case CO_STATUS_INIT:  // await
        continue;
      default:  // yield
        cogo_co_sched_push((cogo_co_sched_t*)sched, (cogo_co_t*)sched->stack_top);
        goto exit_next;
    }
  }
exit_next:
  sched->stack_top = (cogo_await_t*)cogo_co_sched_pop((cogo_co_sched_t*)sched);
exit:
  return sched->stack_top;
}

void cogo_co_run(cogo_co_t* const co) {
  cogo_co_sched_t sched = {
      .super = {
          .stack_top = (cogo_await_t*)co,
      },
  };
  while (cogo_co_sched_step(&sched.super)) {
    // noop
  }
}
