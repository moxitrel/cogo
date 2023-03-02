#include <cogo/cogo_await.h>

#ifdef __GNUC__
#define COGO_FALLTHROUGH __attribute__((fallthrough))
#else
#define COGO_FALLTHROUGH /* [[fallthrough]] */
#endif

static cogo_await_t* cogo_await_sched_step_until_yield(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched && sched->stack_top);
  for (;;) {
    sched->stack_top->sched = sched;
    sched->stack_top->func(sched->stack_top);
    if (!sched->stack_top) {
      // blocked
      if (!(sched->stack_top = cogo_await_sched_pop(sched))) {
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
        cogo_await_sched_push(sched, sched->stack_top);
        goto exit_next;
    }
  }
exit_next:
  sched->stack_top = cogo_await_sched_pop(sched);
exit:
  return sched->stack_top;
}

#if 0
static cogo_await_t* cogo_await_sched_step_once(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched && sched->stack_top);
  sched->stack_top->sched = sched;
  sched->stack_top->func(sched->stack_top);
  if (!sched->stack_top) {
    // blocked
    goto exit_next;
  }
  switch (co_status(sched->stack_top)) {
    case CO_STATUS_FINI:  // return
      if (!(sched->stack_top = sched->stack_top->caller)) {
        goto exit_next;
      }
      COGO_FALLTHROUGH;
    case CO_STATUS_INIT:  // await
      goto exit;
    default:  // yield
      cogo_await_sched_push(sched, sched->stack_top);
      goto exit_next;
  }
exit_next:
  sched->stack_top = cogo_await_sched_pop(sched);
exit:
  return sched->stack_top;
}
#endif

cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* const sched) {
  return cogo_await_sched_step_until_yield(sched);
}
