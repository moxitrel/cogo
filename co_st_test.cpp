#include <cassert>
#include <gtest/gtest.h>
#include "co_st.h"

CO_DECLARE(static recv, co_chan_t* chan, co_msg_t msg) {
    auto& self = *static_cast<recv_t*>(CO_THIS);
    auto& chan = self.chan;
    auto& msg = self.msg;
CO_BEGIN:
    CO_CHAN_READ(chan, &msg);
CO_END:;
}

CO_DECLARE(static send, co_chan_t* chan, co_msg_t msg) {
    auto& self = *static_cast<send_t*>(CO_THIS);
    auto& chan = self.chan;
    auto& msg = self.msg;
CO_BEGIN:
    CO_CHAN_WRITE(chan, &msg);
CO_END:;
}

CO_DECLARE(static main, recv_t recv, send_t send) {
    auto& self = *static_cast<main_t*>(CO_THIS);
    auto& recv = self.recv;
    auto& send = self.send;
CO_BEGIN:
    CO_START(&recv);
    CO_START(&send);
CO_END:;
}

TEST(CogoChan, CoChan) {
    auto chan = CO_CHAN_MAKE(0);
    auto main = CO_MAKE(main, CO_MAKE(recv, &chan), CO_MAKE(send, &chan));
    co_run(&main);
    EXPECT_EQ(&main.send.msg, main.recv.msg.next);
}
