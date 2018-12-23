#include "../../src/coroutine/gen_switch.h"
#include <stdio.h>
#include "../../src/nstd.h"

//
// Used as generator
//

// define coroutine generate_n()
typedef struct {
    // co_t must be inherited (as first field) by user-defined type.
    co_t co;

    // user definitions
    uint i;   // current generated number
    uint max; // stop when i == max
} generate_n_t;

// An generator return i, i+1, ..., max
void generate_n(generate_n_t *co)
{
    // 协程开始
    co_begin(co, 28);

    // return i, i+1, ..., max
    for (; co->i < co->max; co->i++) {
        printf("%i ", co->i);
        co_return(co);
    }

    // 协程结束
    co_end(co);
}

// print 1,...,9 to stdout
void generate_example(void)
{
    // init coroutine
    generate_n_t co = (generate_n_t){
            .co  = CO(generate_n),
            .i   = 1, // start from 1
            .max = 9, // stop  at   9
    };

    co_run(&co);
    printf("\n");
}

int main(void)
{
    generate_example();
    return 0;
}

// clang -std=c17 co_switch_goto_example.c -o /tmp/x && /tmp/x
