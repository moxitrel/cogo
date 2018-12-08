#include "co_switch_goto.h"
#include <stdio.h>

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
    co_begin(co, 26);

    // return i, i+1, ..., max
    for (; co->i < co->max; co->i++) {
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
            .i   = 1, // start from 1
            .max = 9, // stop  at   9
    };

    // run co until finished (when co_state() < 0)
    while (co_state(&co) >= 0) {
        // generate once
        generate_n(&co);
        // print, the value is in co.i
        printf("%i ", co.i);
    }
    printf("\n");
}

int main(void)
{
    generate_example();
    return 0;
}

// clang -std=c11 co_switch_goto_example.c -o /tmp/x && /tmp/x
