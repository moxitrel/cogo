#if defined __GNUC__
#   include "gen_label_value.h"
#elif defined COGOTO_GEN_LINENO
#   include "gen_lineno.h"
#else
#   include "gen_case.h"
#endif
