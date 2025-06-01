#include <assert.h>
#include <cogo/cogo_async.h>
#include <unity.h>

typedef struct ng {
    COGO_T cogo;
    int v;
} ng_t;

static void ng_func(COGO_T* COGO_THIS) {
    ng_t* thiz = (ng_t*)COGO_THIS;
CO_BEGIN:

    for (;; thiz->v++) {
        CO_YIELD;
    }

CO_END:;
}

static ng_t ng_init(ng_t* thiz, int v) {
    return *thiz = (ng_t){
                   .cogo = COGO_INIT(&thiz->cogo, ng_func),
                   .v = v,
           };
}

static int ng_resume(ng_t* thiz) {
    COGO_RESUME(&thiz->cogo);
    return thiz->v;
}

static void test_ng(void) {
    ng_t ng = ng_init(&ng, 0);
    TEST_ASSERT_EQUAL_INT(0, ng_resume(&ng));
    TEST_ASSERT_EQUAL_INT(1, ng_resume(&ng));
    TEST_ASSERT_EQUAL_INT(2, ng_resume(&ng));
}

typedef struct consume {
    COGO_T cogo;
    cogo_chan_t* chan;
    cogo_msg_t msg;
} consume_t;

static void consume_func(COGO_T* COGO_THIS) {
    consume_t* thiz = (consume_t*)COGO_THIS;
CO_BEGIN:

    CO_CHAN_READ(thiz->chan, &thiz->msg);

CO_END:;
}

static consume_t consume_init(consume_t* thiz, cogo_chan_t* chan) {
    return *thiz = (consume_t){
                   .cogo = COGO_INIT(&thiz->cogo, consume_func),
                   .chan = chan,
           };
}

typedef struct product {
    COGO_T cogo;
    cogo_chan_t* chan;
    cogo_msg_t msg;
} product_t;

static void product_func(COGO_T* COGO_THIS) {
    product_t* thiz = (product_t*)COGO_THIS;
CO_BEGIN:

    CO_CHAN_WRITE(thiz->chan, &thiz->msg);

CO_END:;
}

static product_t product_init(product_t* thiz, cogo_chan_t* chan) {
    return *thiz = (product_t){
                   .cogo = COGO_INIT(&thiz->cogo, product_func),
                   .chan = chan,
           };
}

typedef struct entry {
    COGO_T cogo;
    consume_t* r;
    product_t* w;
} entry_t;

static void entry_func(COGO_T* COGO_THIS) {
    entry_t* thiz = (entry_t*)COGO_THIS;
CO_BEGIN:

    CO_ASYNC(&thiz->r->cogo);
    CO_ASYNC(&thiz->w->cogo);

CO_END:;
}

static entry_t entry_init(entry_t* thiz, consume_t* r, product_t* w) {
    return *thiz = (entry_t){
                   .cogo = COGO_INIT(&thiz->cogo, entry_func),
                   .r = r,
                   .w = w,
           };
}

static void test_chan(void) {
    cogo_chan_t c = COGO_CHAN_INIT(0);
    consume_t r = consume_init(&r, &c);
    product_t w = product_init(&w, &c);
    entry_t m = entry_init(&m, &r, &w);
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
