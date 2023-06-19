#include <assert.h>
#include <cogo/cogo_co.h>
#include <unity.h>

CO_DECLARE(/*NAME*/ nat, /*return*/ int v) {
  nat_t* const thiz = (nat_t*)co_this;
CO_BEGIN:

  for (thiz->v = 0;; thiz->v++) {
    CO_YIELD;
  }

CO_END:;
}

void test_nat(void) {
  nat_t n = CO_MAKE(/*NAME*/ nat);  // "v" isn't explicitly initialized

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(0, n.v);

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(1, n.v);

  CO_RESUME(&n);
  TEST_ASSERT_EQUAL_INT(2, n.v);
}

CO_DECLARE(recv, co_channel_t* chan, co_message_t msg) {
  recv_t* const thiz = (recv_t*)co_this;
CO_BEGIN:

  CO_CHANNEL_READ(thiz->chan, &thiz->msg);

CO_END:;
}

CO_DECLARE(send, co_channel_t* chan, co_message_t msg) {
  send_t* const thiz = (send_t*)co_this;
CO_BEGIN:

  CO_CHANNEL_WRITE(thiz->chan, &thiz->msg);

CO_END:;
}

CO_DECLARE(main, recv_t recv, send_t send) {
  main_t* const thiz = (main_t*)co_this;
CO_BEGIN:

  CO_START(&thiz->recv);
  CO_START(&thiz->send);

CO_END:;
}

static void test_chan(void) {
  co_channel_t chan = CO_CHANNEL_MAKE(0);
  main_t main = CO_MAKE(main, CO_MAKE(recv, &chan), CO_MAKE(send, &chan));
  CO_RUN(&main);
  TEST_ASSERT_EQUAL_PTR(&main.send.msg, main.recv.msg.next);
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
