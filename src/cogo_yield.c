#include <cogo/cogo_yield.h>

co_status_t cogo_yield_resume(cogo_yield_t* const co) {
  COGO_ASSERT(co && co->func);
  if (CO_STATUS(co) != CO_STATUS_END) {
    co->func(co);
  }
  return CO_STATUS(co);
}

void cogo_yield_run(cogo_yield_t* const co) {
  COGO_ASSERT(co && co->func);
  while (CO_STATUS(co) != CO_STATUS_END) {
    co->func(co);
  }
}
