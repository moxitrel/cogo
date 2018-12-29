//
// 1. Include header "coroutine.h"
//
#include "coroutine.h"
#include <stdio.h>

//
// 2. Define a class that inherit co_t
//
// A natural number generator
struct Nat : public co_t {
    //
    // Define local variables, parameters, return values of coroutine function
    //
    unsigned int value;

    //
    // 3. Override operator() with the type "void()"
    //
    void operator()()
    {
        //
        // 4. Set coroutine begin
        //
        co_begin(32);
     // co_begin();         // you can omit line numbers if enable GNUC extension

        //
        // 5. User codes
        //
        for (value = 0; ; value++) {
            co_yield();
        }

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
    Nat nat;

    nat(); // nat.value: 0
    printf("nat.value: %d\n", nat.value);

    nat(); // nat.value: 1
    printf("nat.value: %d\n", nat.value);

    nat(); // nat.value: 2
    printf("nat.value: %d\n", nat.value);

    return 0;
}