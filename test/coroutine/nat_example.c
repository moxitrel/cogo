#undef __GNUC__
#include "../../src/coroutine/coroutine.h"
// natural number generator
typedef struct {
    co_t co;

    //
    int value;  // declare local variables, return values as struct filed
    //
} nat_t;

void nat(nat_t *co)
{
    co_begin(co, 18);

    //
    for (co->value = 0; ; co->value ++) {   //
        co_yield(co);                       // user code
    }                                       //
    //

    co_end(co);
}

int main()
{
    nat_t x = {
            .co = CO(nat),
    };
    nat(&x);    // x.value: 0
    nat(&x);    // x.value: 1
    nat(&x);    // x.value: 2
}