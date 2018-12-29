#include "../../src/coroutine/gen_switch.hpp"
#undef __GNUC__
struct T : public gen_t {   // inherit
    int i;                  // as local variables

    void f()                // coroutine function
    {
        co_begin(11);       // begin

        for (i = 0; i < 9; i++) {
            co_yield();     // yield
        }

        co_end();           // coroutine end
    }
};
