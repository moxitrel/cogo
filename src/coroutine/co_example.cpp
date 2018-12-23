#include "co.hpp"               // 1. include co.hpp
#include <stdio.h>

class CoroutineExample1 : public co_t {
    // a coroutine print 1:0, 1:1, ... 1:5
    class : public co_t {       // 2. inherent co_t
        int i;
        void operator()()       // 3. override operator(), which has the type "void ()"
        {
            co_begin(14);       // 4. mark coroutine begin

            for (i = 0; i < 5; i++) {
                printf("1:%d\n", i);
                co_return();    // yield
            }

            co_end();           // 4. mark coroutine end
        }
    } coroutine1;

    // a coroutine print 2:0, 2:1, ... 2:5
    class : public co_t {
        int i;
        void operator()()
        {
            co_begin(27);               // begin
            for (i = 0; i < 5; i++) {
                printf("2:%d\n", i);
                co_return();            // yield
            }
            co_end();                   // end
        }
    } coroutine2;

    // run coroutine1, coroutine2 concurrently
    void operator()()
    {
        co_begin(38,39);

        co_sched(coroutine1);   // schedule to run coroutine1 concurrently
        co_sched(coroutine2);   // schedule to run coroutine2 concurrently

        co_end();
    }
};

int main()
{
    // Run until finish all coroutines.
    // Print:
    //  1:0
    //  1:1
    //  2:0
    //  1:2
    //  2:1
    //  1:3
    //  ...
    CoroutineExample1().run();
}

// clang -std=c++17 co_example.cpp -lstdc++ -o /tmp/CoroutineExample1 && /tmp/CoroutineExample1
