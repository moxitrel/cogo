#include <assert.h>
#include <cogo/cogo_yield.h>
#include <unity.h>

COGO_DECLARE(static ng_t, int v) {
    ng_t* const thiz = (ng_t*)COGO_THIS;
CO_BEGIN:

    for (;; thiz->v++) {
        CO_YIELD;
    }

CO_END:;
}

static void test_ng(void) {
    ng_t ng = {
            .COGO_THIS = COGO_INIT(ng_t, &ng),
            .v = 0,
    };

    COGO_RESUME(&ng);
    TEST_ASSERT_EQUAL_INT(0, ng.v);

    COGO_RESUME(&ng);
    TEST_ASSERT_EQUAL_INT(1, ng.v);

    COGO_RESUME(&ng);
    TEST_ASSERT_EQUAL_INT(2, ng.v);
}

COGO_DECLARE(ng_t1, int v);
COGO_DEFINE(ng_t1) {
    ng_t1* const thiz = (ng_t1*)COGO_THIS;
CO_BEGIN:

    for (;; thiz->v++) {
        CO_YIELD;
    }

CO_END:;
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ng);

    return UNITY_END();
}
