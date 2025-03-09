#include <cogo/cogo_yield.h>

cogo_pc_t cogo_yield_resume(cogo_yield_t* const COGO_THIS) {
  COGO_ASSERT(cogo_yield_is_valid(COGO_THIS));
  if (COGO_PC(COGO_THIS) != COGO_PC_END) {
    COGO_THIS->func(COGO_THIS);
  }
  return COGO_PC(COGO_THIS);
}
