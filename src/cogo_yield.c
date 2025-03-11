#include <cogo/cogo_yield.h>

cogo_pc_t cogo_yield_resume(cogo_yield_t* const cogo) {
  COGO_ASSERT(cogo_yield_is_valid(cogo));
  if (COGO_PC(cogo) != COGO_PC_END) {
    cogo->func(cogo);
  }
  return COGO_PC(cogo);
}
