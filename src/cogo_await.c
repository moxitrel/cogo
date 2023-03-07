#include <cogo/cogo_await.h>

#ifdef __GNUC__
#define COGO_FALLTHROUGH __attribute__((fallthrough))
#else
#define COGO_FALLTHROUGH /* [[fallthrough]] */
#endif

// run until yield
cogo_await_t* cogo_await_sched_step(cogo_await_sched_t* const sched) {
  COGO_ASSERT(sched && sched->stack_top);
  for (;;) {
    sched->stack_top->sched = sched;
    sched->stack_top->func(sched->stack_top);
    switch (co_status(sched->stack_top)) {
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
