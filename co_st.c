#include "co_st.h"

extern inline cogo_co_t* cogo_sch_step(cogo_sch_t* sch);
extern inline int cogo_sch_add(cogo_sch_t* sch, cogo_co_t* co);
extern inline cogo_co_t* cogo_sch_rm(cogo_sch_t* sch);

extern inline int cogo_chan_read(co_t* co, co_chan_t* chan, co_msg_t* msg_next);
extern inline int cogo_chan_write(co_t* co, co_chan_t* chan, co_msg_t* msg);
