#include <cassert>
#include <gtest/gtest.h>
#include "cogo_yield.h"

CO_DECLARE(static coyield, int v) {
CO_BEGIN:
    ((coyield_t*)CO_THIS)->v++;
    CO_YIELD;
    ((coyield_t*)CO_THIS)->v++;
CO_END:;
}

TEST(CogoYield, CoYield) {
    coyield_t coyield = CO_MAKE(coyield, 0);
    EXPECT_EQ(CO_STATUS(&coyield), COGO_STATUS_STARTED);  // init
    EXPECT_EQ(coyield.v, 0);

    coyield_func(&coyield);
    EXPECT_NE(CO_STATUS(&coyield), COGO_STATUS_STARTED);  // running
    EXPECT_NE(CO_STATUS(&coyield), COGO_STATUS_STOPPED);  // running
    EXPECT_EQ(coyield.v, 1);

    coyield_func(&coyield);
    EXPECT_EQ(CO_STATUS(&coyield), COGO_STATUS_STOPPED);  // end
    EXPECT_EQ(coyield.v, 2);

    // noop when coroutine end
    coyield_func(&coyield);
    EXPECT_EQ(CO_STATUS(&coyield), COGO_STATUS_STOPPED);
    EXPECT_EQ(coyield.v, 2);
}

CO_DECLARE(static coreturn, int v) {
CO_BEGIN:
    ((coreturn_t*)CO_THIS)->v++;
    CO_RETURN;
    ((coreturn_t*)CO_THIS)->v++;
CO_END:;
}

TEST(CogoYield, CoReturn) {
    coreturn_t coreturn = CO_MAKE(coreturn, 0);
    EXPECT_EQ(CO_STATUS(&coreturn), COGO_STATUS_STARTED);  // init
    EXPECT_EQ(coreturn.v, 0);

    coreturn_func(&coreturn);
    EXPECT_EQ(CO_STATUS(&coreturn), COGO_STATUS_STOPPED);  // end
    EXPECT_EQ(coreturn.v, 1);
}

CO_DECLARE(static prologue, int enter, int exit) {
    prologue_t* thiz = (prologue_t*)CO_THIS;
    thiz->enter++;
CO_BEGIN:

    CO_YIELD;
    CO_YIELD;

CO_END:
    thiz->exit++;
}

TEST(CogoYield, Prologue) {
    prologue_t prologue = CO_MAKE(prologue, 0, 0);

    while (CO_STATUS(&prologue) != COGO_STATUS_STOPPED) {
        prologue_func(&prologue);
    }
    EXPECT_EQ(prologue.enter, 3);
    EXPECT_EQ(prologue.exit, 3);

    prologue_func(&prologue);
    EXPECT_EQ(prologue.enter, 4);
    EXPECT_EQ(prologue.exit, 4);
}
