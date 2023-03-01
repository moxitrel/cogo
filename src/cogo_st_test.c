#include <assert.h>
#include <cogo/cogo_st.h>
#include <unity.h>

CO_DECLARE(recv, co_chan_t* chan, co_msg_t msg) {
  recv_t* thiz = (recv_t*)co_this;
CO_BEGIN:
  CO_CHAN_READ(thiz->chan, &thiz->msg);
CO_END:;
}

CO_DECLARE(send, co_chan_t* chan, co_msg_t msg) {
  send_t* thiz = (send_t*)co_this;
CO_BEGIN:
  CO_CHAN_WRITE(thiz->chan, &thiz->msg);
CO_END:;
}

CO_DECLARE(main, recv_t recv, send_t send) {
  main_t* thiz = (main_t*)co_this;
CO_BEGIN:
  CO_START(&thiz->recv);
  CO_START(&thiz->send);
CO_END:;
}

static void test_chan(void) {
  co_chan_t chan = CO_CHAN_MAKE(0);
  main_t main = CO_MAKE(main, CO_MAKE(recv, &chan), CO_MAKE(send, &chan));
  cogo_st_run(&main);
  TEST_ASSERT_EQUAL_PTR(&main.send.msg, main.recv.msg.next);
}

void setUp(void) {
}

void tearDown(void) {
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_chan);

  return UNITY_END();
}
