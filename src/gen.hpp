#if defined(COGO_CASE)
#   include "gen_case.hpp"
#elif defined(COGO_LABEL_VALUE)
#   include "gen_label_value.hpp"
#elif defined(COGO_LINE_NO)
#   include "gen_line_no.hpp"
#elif defined(__GNUC__)
#   include "gen_label_value.hpp"
#else
#   include "gen_case.hpp"
#endif
