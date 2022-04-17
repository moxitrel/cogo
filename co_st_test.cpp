#include "co_st.h"
#include <assert.h>
#include "gtest/gtest.h"

CO_DECLARE(static recv, co_chan_t* chan, co_msg_t msg) {
    auto& self = *static_cast<recv_t*>(CO_THIS);
CO_BEGIN:
    CO_CHAN_READ(self.chan, &self.msg);
CO_END:;
}

CO_DECLARE(static send, co_chan_t* chan, co_msg_t msg) {
    auto& self = *static_cast<send_t*>(CO_THIS);
CO_BEGIN:
    CO_CHAN_WRITE(self.chan, &self.msg);
CO_END:;
}

CO_DECLARE(static main, recv_t recv, send_t send) {
    auto& self = *static_cast<main_t*>(CO_THIS);
CO_BEGIN:
    CO_START(&self.recv);
    CO_START(&self.send);
CO_END:;
}

TEST(CogoChan, CoChan) {
    auto chan = CO_CHAN_MAKE(0);
    auto main = CO_MAKE(main, CO_MAKE(recv, &chan), CO_MAKE(send, &chan));
    co_run(&main);
    EXPECT_EQ(&main.send.msg, main.recv.msg.next);
}
