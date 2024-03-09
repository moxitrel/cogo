#include <assert.h>
#include <cogo/cogo_async.h>
#include <unity.h>

CO_DECLARE(/*FUNC*/ nat, /*return*/ int v) {
  nat_t* const thiz = (nat_t*)co_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

static void test_nat(void) {
  nat_t n = CO_INIT(/*FUNC*/ nat, /*this*/ &n);  // "v" isn't explicitly initialized

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(2, n.v);
}

CO_DECLARE(consume, co_chan_t* chan, co_message_t msg) {
  consume_t* const thiz = (consume_t*)co_this;
CO_BEGIN:

  CO_CHAN_READ(thiz->chan, &thiz->msg);

CO_END:;
}

CO_DECLARE(product, co_chan_t* chan, co_message_t msg) {
  product_t* const thiz = (product_t*)co_this;
CO_BEGIN:

  CO_CHAN_WRITE(thiz->chan, &thiz->msg);

CO_END:;
}

CO_DECLARE(entry, consume_t* r, product_t* w) {
  entry_t* const thiz = (entry_t*)co_this;
CO_BEGIN:

  CO_ASYNC(thiz->r);
  CO_ASYNC(thiz->w);

CO_END:;
}

static void test_chan(void) {
  co_chan_t c = CO_CHAN_MAKE(0);
  consume_t r = CO_INIT(/*FUNC*/ consume, /*this*/ &r, .chan = &c);
  product_t w = CO_INIT(/*FUNC*/ product, /*this*/ &w, .chan = &c);
  entry_t m = CO_INIT(/*FUNC*/ entry, /*this*/ &m, .w = &w, .r = &r);
  CO_RUN(&m);
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
