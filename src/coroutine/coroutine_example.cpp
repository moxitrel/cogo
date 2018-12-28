//
// 1. Include header "coroutine.h"
//
#include <stdio.h>
#include "coroutine.h"

//
// 2. Define a class that inherit co_t
//
// Print 0x7ffee80:0, 0x7ffee80:1, ... 0x7ffee80:6
class PrintN : public co_t {
    //
    // Declare local variables, parameters, return values for coroutine function
    //
    int i;

    //
    // 3. Override operator(), which has the type "void ()"
    //
    void operator()()
    {
        //
        // 4. Set coroutine begin
        //
        co_begin(33);       // 33: list the line numbers of co_return(), co_call(), co_sched(), i.e. the value of __LINE__
        // co_begin();         // you can omit line numbers if enable GNUC extension

        //
        // 5. User code (*** No stack variable allowed, use member variable instead ***)
        //
        for (i = 0; i < 7; i++) {
            printf("%p:%d\n", this, i);
            co_return();    // yield
        }

        //
        // 4. Set coroutine end
        //
        co_end();
    }
};

// Create two PrintN coroutine
class CoroutineExample : public co_t {  // 2. Define a class that inherit co_t
    PrintN coroutine1;          // Declare local variables of coroutine function
    PrintN coroutine2;          //

    void operator()()           // 3. Override operator()
    {
        co_begin(56,57);        // 4. Coroutine begin
        // co_begin();

        //
        // 5. User code
        //
        co_sched(coroutine1);   // add coroutine1 to scheduler
        co_sched(coroutine2);   // add coroutine2 to scheduler

        co_end();               // 4. Coroutine end
    }
};

// 6. Use it
int main()
{
    // Run until finish all coroutines.
    // Output:
    //  0x8f0:0
    //  0x8f0:1
    //  0x918:0
    //  0x8f0:2
    //  0x918:1
    //  0x8f0:3
    //  ...
    CoroutineExample().run();
}