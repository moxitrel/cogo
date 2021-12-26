#include "co_st.h"
#include <assert.h>
#include "gtest/gtest.h"

CO_DECLARE(static Recv, co_chan_t* c, co_msg_t msgNext) {
    auto* thiz = static_cast<Recv*>(CO_THIS);
CO_BEGIN:
    CO_CHAN_READ(thiz->c, &thiz->msgNext);
CO_END:;
}

CO_DECLARE(static Send, co_chan_t* c, co_msg_t msg) {
    auto& self = *static_cast<Send*>(CO_THIS);
CO_BEGIN:
    CO_CHAN_WRITE(self.c, &self.msg);
CO_END:;
}

CO_DECLARE(static Entry, Recv recv1, Send send1) {
    auto& self = *static_cast<Entry*>(CO_THIS);
CO_BEGIN:
    CO_START(&self.recv1);
    CO_START(&self.send1);
CO_END:;
}

TEST(Chan, ReadWrite) {
    auto c0 = CO_CHAN_MAKE(0);
    auto entry = CO_MAKE(Entry, CO_MAKE(Recv, .c=&c0), CO_MAKE(Send, .c=&c0));
    co_run(&entry);
    EXPECT_EQ(&entry.send1.msg, entry.recv1.msgNext.next);
}
