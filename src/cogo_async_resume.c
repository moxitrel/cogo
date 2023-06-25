#include <cogo/cogo_async.h>

// run until yield
// NOTE: .entry is damaged after call
cogo_async_t* cogo_async_sched_resume(cogo_async_sched_t* const sched) {
#define CALL_TOP (((cogo_await_sched_t*)sched)->call_top)
  COGO_ASSERT(sched && CALL_TOP);

  for (;;) {
    CALL_TOP->sched = (cogo_await_sched_t*)sched;
    CALL_TOP->func(CALL_TOP);
    if (!CALL_TOP) {
      // blocked
      if (!(CALL_TOP = (cogo_await_t*)cogo_async_sched_pop(sched))) {
        // no more active coroutines
        goto exit;
      }
      continue;
    }
    switch (co_status((cogo_yield_t*)CALL_TOP)) {
      case CO_STATUS_END:  // return
        if (!(CALL_TOP = CALL_TOP->caller)) {
          // return from root
          goto exit_next;
        }
        continue;
      case CO_STATUS_BEGIN:  // await
        continue;
      default:  // yield
        cogo_async_sched_push(sched, (cogo_async_t*)CALL_TOP);
        goto exit_next;
    }
  }
exit_next:
  CALL_TOP = (cogo_await_t*)cogo_async_sched_pop(sched);
exit:
  return (cogo_async_t*)CALL_TOP;

#undef CALL_TOP
}

#ifdef COGO_USE_RESUME
cogo_async_t* cogo_async_resume(cogo_async_t* const co) {
#define ENTRY (((cogo_await_t*)co)->entry)
  COGO_ASSERT(co);
  
  cogo_async_sched_t sched = {
      .super = {
          .call_top = ENTRY ? ENTRY : (cogo_await_t*)co,
      },
  };
  // repaire .entry
  return (cogo_async_t*)(ENTRY = (cogo_await_t*)cogo_async_sched_resume(&sched));

#undef ENTRY
}
#endif
