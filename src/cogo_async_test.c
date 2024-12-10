#include <assert.h>
#include <cogo/cogo_async.h>
#include <unity.h>

COGO_DECLARE(/*FUNC*/ nat, /*return*/ int v) {
  nat_t* const thiz = (nat_t*)cogo_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_nat(void) {
  nat_t n = {
      .cogo = COGO_INITIALIZER(nat, &n),
  };
  COGO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  COGO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  COGO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(2, n.v);
}

COGO_DECLARE(consume, co_chan_t* chan, co_message_t msg) {
  consume_t* const thiz = (consume_t*)cogo_this;
CO_BEGIN:

  CO_CHAN_READ(thiz->chan, &thiz->msg);

CO_END:;
}

COGO_DECLARE(product, co_chan_t* chan, co_message_t msg) {
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
  co_chan_t c = CO_CHAN_MAKE(0);
  consume_t r = {
      .cogo = COGO_INITIALIZER(consume, &r),
      .chan = &c,
  };
  product_t w = {
      .cogo = COGO_INITIALIZER(product, &w),
      .chan = &c,
  };
  entry_t m = {
      .cogo = COGO_INITIALIZER(entry, &m),
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

  RUN_TEST(test_nat);
  RUN_TEST(test_chan);

  return UNITY_END();
}
