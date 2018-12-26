#include "../../src/coroutine/coroutine.h"

// A natural number generator
struct Nat : public co_t {
    unsigned int value;

    void operator()()
    {
        co_begin(13);
     // co_begin();         // if enable GNUC extension

        for (value = 0; ; value++) {
            co_return();
        }

        co_end();
    }
};

#include <stdio.h>
int main()
{
    Nat nat;
    nat(); // nat.value: 0
    nat(); // nat.value: 1
    nat(); // nat.value: 2

    printf("gen_t.size: %zd\n", sizeof(gen_t));
    printf("fun_t.size: %zd\n", sizeof(fun_t));
    printf("co_t.size: %zd\n", sizeof(co_t));
}