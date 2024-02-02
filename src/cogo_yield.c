#include <cogo/cogo_yield.h>

co_status_t cogo_yield_resume(cogo_yield_t* const co) {
  COGO_ASSERT(co && co->resume);
  if (CO_STATUS(co) != CO_STATUS_END) {
    co->resume(co);
  }
  return CO_STATUS(co);
}

void cogo_yield_run(cogo_yield_t* const co) {
  COGO_ASSERT(co);
  while (cogo_yield_resume(co) != CO_STATUS_END) {
  }
}
  