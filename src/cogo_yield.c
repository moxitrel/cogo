#include <cogo/cogo_yield.h>

cogo_pc_t cogo_yield_resume(cogo_yield_t* const cogo) {
  COGO_ASSERT(cogo && cogo->resume);
  if (COGO_PC(&cogo->base_pt) != COGO_PC_END) {
    cogo->resume(cogo);
  }
  return COGO_PC(&cogo->base_pt);
}
