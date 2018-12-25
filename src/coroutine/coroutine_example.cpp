#include "coroutine.h"      // 1. include coroutine.h
#include <stdio.h>

// a coroutine print 0x7ffee80:0, 0x7ffee80:1, ... 0x7ffee80:6
class Print: public co_t {  // 2. inherit co_t
    // define local variables for coroutine function
    int i;

    void operator()()       // 3. override operator(), which has the type "void ()"
    {
        co_begin(16);       // 4. coroutine begin, 20: the line number of co_return(), co_call(), co_sched()
     // co_begin();         //    you can omit line numbers if enable gnuc extension

        for (i = 0; i < 7; i++) {
            printf("%p:%d\n", this, i);
            co_return();    // 5. yield
        }

        co_end();           // 4. coroutine end
    }
};

// a coroutine run two Print coroutine concurrently
class CoroutineExample : public co_t {  // inherit co_t
    // define local variables
    Print coroutine1;
    Print coroutine2;

    void operator()()           // override operator()
    {
        co_begin(34, 35);
     // co_begin();

        co_sched(coroutine1);   // run coroutine1 concurrently
        co_sched(coroutine2);   // run coroutine2 concurrently

        co_end();
    }
};

int main()
{
    // Run until finish all coroutines.
    // Print:
    //  0x8f0:0
    //  0x8f0:1
    //  0x918:0
    //  0x8f0:2
    //  0x918:1
    //  0x8f0:3
    //  ...
    CoroutineExample().run();
}

// clang -std=c++17 coroutine_example.cpp -lstdc++ -o /tmp/CoroutineExample && /tmp/CoroutineExample
