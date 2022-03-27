#include <assert.h>

#if defined(COGO_CASE)
    #include "cogo_yield_case.h"
#elif defined(COGO_LABEL_VALUE)
    #include "cogo_yield_label_value.h"
#endif

#include "cogo_yield.h"
#include "gtest/gtest.h"

CO_DECLARE(static coyield, int v) {
CO_BEGIN:
    ((coyield_t*)CO_THIS)->v++;
    CO_YIELD;
    ((coyield_t*)CO_THIS)->v++;
CO_END:;
}

TEST(cogo_yield_t, Yield) {
    coyield_t coyield1 = CO_MAKE(coyield, 0);
    EXPECT_EQ(CO_STATUS(&coyield1), COGO_STATUS_STARTED);  // init
    EXPECT_EQ(coyield1.v, 0);

    coyield_func(&coyield1);
    EXPECT_NE(CO_STATUS(&coyield1), COGO_STATUS_STARTED);  // running
    EXPECT_NE(CO_STATUS(&coyield1), COGO_STATUS_STOPPED);  // running
    EXPECT_EQ(coyield1.v, 1);

    coyield_func(&coyield1);
    EXPECT_EQ(CO_STATUS(&coyield1), COGO_STATUS_STOPPED);  // end
    EXPECT_EQ(coyield1.v, 2);

    // noop when coroutine end
    coyield_func(&coyield1);
    EXPECT_EQ(CO_STATUS(&coyield1), COGO_STATUS_STOPPED);
    EXPECT_EQ(coyield1.v, 2);
}

CO_DECLARE(static coreturn, int v) {
CO_BEGIN:
    ((coreturn_t*)CO_THIS)->v++;
    CO_RETURN;
    ((coreturn_t*)CO_THIS)->v++;
CO_END:;
}

TEST(cogo_yield_t, Return) {
    coreturn_t coreturn1 = CO_MAKE(coreturn, 0);
    EXPECT_EQ(CO_STATUS(&coreturn1), COGO_STATUS_STARTED);  // init
    EXPECT_EQ(coreturn1.v, 0);

    coreturn_func(&coreturn1);
    EXPECT_EQ(CO_STATUS(&coreturn1), COGO_STATUS_STOPPED);  // end
    EXPECT_EQ(coreturn1.v, 1);
}

CO_DECLARE(prologue, int enter, int exit);
CO_DEFINE(prologue) {
    prologue_t* thiz = (prologue_t*)CO_THIS;
    thiz->enter++;

CO_BEGIN:
    CO_YIELD;
    CO_YIELD;
CO_END:

    thiz->exit++;
}

TEST(cogo_yield_t, Prologue) {
    prologue_t prologue = CO_MAKE(prologue, 0, 0);

    while (CO_STATUS(&prologue) != COGO_STATUS_STOPPED) {
        prologue_func(&prologue);
        std::printf("%jx\n", (uintmax_t)CO_STATUS(&prologue));
    }

    EXPECT_EQ(prologue.enter, 3);
    EXPECT_EQ(prologue.exit, 3);
}
