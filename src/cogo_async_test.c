#include <assert.h>
#include <cogo/cogo_async.h>
#include <unity.h>

COGO_DECLARE(ng_t, int v) {
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

COGO_DECLARE(consume_t, cogo_chan_t* chan, cogo_msg_t msg) {
    consume_t* const thiz = (consume_t*)COGO_THIS;
CO_BEGIN:

    CO_CHAN_READ(thiz->chan, &thiz->msg);

CO_END:;
}

COGO_DECLARE(product_t, cogo_chan_t* chan, cogo_msg_t msg) {
    product_t* const thiz = (product_t*)COGO_THIS;
CO_BEGIN:

    CO_CHAN_WRITE(thiz->chan, &thiz->msg);

CO_END:;
}

COGO_DECLARE(entry_t, consume_t* r, product_t* w) {
    entry_t* const thiz = (entry_t*)COGO_THIS;
CO_BEGIN:

    CO_ASYNC(thiz->r);
    CO_ASYNC(thiz->w);

CO_END:;
}

static void test_chan(void) {
    cogo_chan_t c = COGO_CHAN_INIT(0);
    consume_t r = {
            .COGO_THIS = COGO_INIT(consume_t, &r),
            .chan = &c,
    };
    product_t w = {
            .COGO_THIS = COGO_INIT(product_t, &w),
            .chan = &c,
    };
    entry_t m = {
            .COGO_THIS = COGO_INIT(entry_t, &m),
            .w = &w,
            .r = &r,
    };
    COGO_RUN(&m);
    TEST_ASSERT_EQUAL_PTR(&w.msg, r.msg.next);
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ng);
    RUN_TEST(test_chan);

    return UNITY_END();
}
