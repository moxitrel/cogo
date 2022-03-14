#include "co_st.h"
#include <assert.h>
#include "gtest/gtest.h"

CO_DECLARE(static recv, co_chan_t* c, co_msg_t msgNext) {
    auto* thiz = static_cast<recv_t*>(CO_THIS);
CO_BEGIN:
    CO_CHAN_READ(thiz->c, &thiz->msgNext);
CO_END:;
}

CO_DECLARE(static send, co_chan_t* c, co_msg_t msg) {
    auto& self = *static_cast<send_t*>(CO_THIS);
CO_BEGIN:
    CO_CHAN_WRITE(self.c, &self.msg);
CO_END:;
}

CO_DECLARE(static entry, recv_t recv1, send_t send1) {
    auto& self = *static_cast<entry_t*>(CO_THIS);
CO_BEGIN:
    CO_START(&self.recv1);
    CO_START(&self.send1);
CO_END:;
}

TEST(co_chan_t, ReadWrite) {
    auto c0 = CO_CHAN_MAKE(0);
    auto entry = CO_MAKE(entry, CO_MAKE(recv, .c = &c0), CO_MAKE(send, .c = &c0));
    co_run(&entry);
    EXPECT_EQ(&entry.send1.msg, entry.recv1.msgNext.next);
}
