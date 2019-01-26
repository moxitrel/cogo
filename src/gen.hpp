#if defined __GNUC__
#   include "gen_label_value.hpp"
#elif defined COGOTO_GEN_LINENO
#   include "gen_lineno.hpp"
#else
#   include "gen_case.hpp"
#endif
