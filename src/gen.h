#if defined(COGO_CASE)
#   include "gen_case.h"
#elif defined(COGO_LABEL_VALUE)
#   include "gen_label_value.h"
#elif defined(COGO_LINE_NO)
#   include "gen_lineno.h"
#elif defined(__GNUC__)
#   include "gen_label_value.h"
#else
#   include "gen_case.h"
#endif
