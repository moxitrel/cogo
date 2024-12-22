#include <assert.h>
#include <cogo/cogo_async.h>
#include <unity.h>

COGO_DECLARE(ng, int v) {
  ng_t* const thiz = (ng_t*)cogo_this;
CO_BEGIN:

  for (;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_ng(void) {
  ng_t ng = {
      .cogo = COGO_INIT(ng, &ng),
  };
  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(0, ng.v);

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(1, ng.v);

  COGO_RESUME(&ng);
  TEST_ASSERT_EQUAL_INT(2, ng.v);
}

COGO_DECLARE(consume, cogo_chan_t* chan, cogo_msg_t msg) {
  consume_t* const thiz = (consume_t*)cogo_this;
CO_BEGIN:

  CO_CHAN_READ(thiz->chan, &thiz->msg);

CO_END:;
}

COGO_DECLARE(product, cogo_chan_t* chan, cogo_msg_t msg) {
  product_t* const thiz = (product_t*)cogo_this;
CO_BEGIN:

  CO_CHAN_WRITE(thiz->chan, &thiz->msg);

CO_END:;
}

COGO_DECLARE(entry, consume_t* r, product_t* w) {
  entry_t* const thiz = (entry_t*)cogo_this;
CO_BEGIN:

  CO_ASYNC(thiz->r);
  CO_ASYNC(thiz->w);

CO_END:;
}

static void test_chan(void) {
  cogo_chan_t c = COGO_CHAN_INIT(0);
  consume_t r = {
      .cogo = COGO_INIT(consume, &r),
      .chan = &c,
  };
  product_t w = {
      .cogo = COGO_INIT(product, &w),
      .chan = &c,
  };
  entry_t m = {
      .cogo = COGO_INIT(entry, &m),
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
