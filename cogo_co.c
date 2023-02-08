#include "cogo_co.h"

cogo_co_t* cogo_sch_step(cogo_sch_t* sch) {
    COGO_ASSERT(sch);
    while (sch->stack_top) {
        sch->stack_top->sch = sch;
        sch->stack_top->func(sch->stack_top);
        if (!sch->stack_top) {
            // blocked
            break;
        }
        switch (CO_STATUS(sch->stack_top)) {
        case COGO_STATUS_STARTED:  // await
            continue;
        case COGO_STATUS_STOPPED:  // return
            sch->stack_top = sch->stack_top->caller;
            continue;
        default:  // yield
            cogo_sch_push(sch, sch->stack_top);
            break;
        }
        break;
    }
    return sch->stack_top = cogo_sch_pop(sch);
}
