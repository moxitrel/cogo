//
// 1. Include header "coroutine.h"
//
#include "coroutine.h"
#include <stdio.h>

//
// 2. Define a class that inherit co_t
//
// A coroutine print 0x7ffee80:0, 0x7ffee80:1, ... 0x7ffee80:6
class Print: public co_t {
    //
    // Define local variables, parameters, return values of coroutine function
    //
    int i;

    //
    // 3. Override operator(), which has the type "void ()"
    //
    void operator()()
    {
        //
        // 4. Set coroutine begin, 33: the line number of co_return(), co_call(), co_sched()
        //
        co_begin(33);
     // co_begin();         // you can omit line numbers if enable GNUC extension

        //
        // 5. User codes
        //
        for (i = 0; i < 7; i++) {
            printf("%p:%d\n", this, i);
            co_return();    // yield
        }

        //
        // 4. Set coroutine end
        //
        co_end();           // 4. coroutine end
    }
};

//
// 2. Define a class that inherit co_t
//
// A coroutine create two Print coroutine which run concurrently
class CoroutineExample : public co_t {
    //
    // Define local variables, parameters, return values of coroutine function
    //
    Print coroutine1;
    Print coroutine2;

    //
    // 3. Override operator() with the type "void()"
    //
    void operator()()
    {
        //
        // 4. Set coroutine begin
        //
        co_begin(68, 69);
     // co_begin();             // you can omit line numbers if enable GNUC extension

        //
        // 5. User codes
        //
        co_sched(coroutine1);   // run coroutine1 concurrently
        co_sched(coroutine2);   // run coroutine2 concurrently

        //
        // 4. Set coroutine end
        //
        co_end();
    }
};

//
// 6. Use it
//
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
