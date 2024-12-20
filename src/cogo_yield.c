#include <cogo/cogo_yield.h>

cogo_pc_t cogo_yield_resume(cogo_yield_t* const cogo_this) {
  COGO_ASSERT(cogo_this && cogo_this->resume);
  if (COGO_PC(cogo_this) != COGO_PC_END) {
    cogo_this->resume(cogo_this);
  }
  return COGO_PC(cogo_this);
}
