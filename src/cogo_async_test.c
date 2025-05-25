#include <assert.h>
#include <cogo/cogo_async.h>
#include <unity.h>

typedef struct ng {
    COGO_T cogo;
    int v;
} ng_t;

static void ng_run(COGO_T* const COGO_THIS) {
    ng_t* const ng_this = (ng_t*)COGO_THIS;
CO_BEGIN:

    for (;; ng_this->v++) {
        CO_YIELD;
    }

CO_END:;
}

static void test_ng(void) {
    ng_t ng = {
            .cogo = COGO_INIT(&ng.cogo, ng_run),
            .v = 0,
    };
    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(0, ng.v);

    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(1, ng.v);

    COGO_RESUME(&ng.cogo);
    TEST_ASSERT_EQUAL_INT(2, ng.v);
}

typedef struct consume {
    COGO_T cogo;
    cogo_chan_t* chan;
    cogo_msg_t msg;
} consume_t;

static void consume_run(COGO_T* const COGO_THIS) {
    consume_t* const consume_thiz = (consume_t*)COGO_THIS;
CO_BEGIN:

    CO_CHAN_READ(consume_thiz->chan, &consume_thiz->msg);

CO_END:;
}

typedef struct product {
    COGO_T cogo;
    cogo_chan_t* chan;
    cogo_msg_t msg;
} product_t;

static void product_run(COGO_T* const COGO_THIS) {
    product_t* const product_thiz = (product_t*)COGO_THIS;
CO_BEGIN:

    CO_CHAN_WRITE(product_thiz->chan, &product_thiz->msg);

CO_END:;
}

typedef struct entry {
    COGO_T cogo;
    consume_t* r;
    product_t* w;
} entry_t;

static void entry_run(COGO_T* const COGO_THIS) {
    entry_t* const entry_thiz = (entry_t*)COGO_THIS;
CO_BEGIN:

    CO_ASYNC(&entry_thiz->r->cogo);
    CO_ASYNC(&entry_thiz->w->cogo);

CO_END:;
}

static void test_chan(void) {
    cogo_chan_t c = COGO_CHAN_INIT(0);
    consume_t r = {
            .cogo = COGO_INIT(&r.cogo, consume_run),
            .chan = &c,
    };
    product_t w = {
            .cogo = COGO_INIT(&w.cogo, product_run),
            .chan = &c,
    };
    entry_t m = {
            .cogo = COGO_INIT(&m.cogo, entry_run),
            .w = &w,
            .r = &r,
    };
    COGO_RUN(&m.cogo);
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
