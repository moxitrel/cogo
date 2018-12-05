#include "co_label_value.h"
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
    co_begin(co);   // no line number need

    // return i, i+1, ..., max
    for (; co->i <= co->max; co->i++) {
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

    // run co until finished, when co_state() < 0
    for (;;) {
        generate_n(&co);
        if (co_state(&co) < 0) { // finished?
            break;
        }
        printf("gen: %d\n", co.i);
    }
}

typedef struct {
    co_t co; // must be inherited
    int  n;  // current number
} print_t;

void print(print_t *co)
{
    // 协程开始
    co_begin(co);   // no line number need

    // return i, i+1, ..., max
    for (; co->n < 10; co->n += 2) {
        printf("%d\n", co->n);
        co_return(co);
    }

    // 协程结束
    co_end(co);
}

void print_init(print_t *o, int n)
{
    *o = (print_t){
        .co = CO(print),
        .n  = n,
    };
}

typedef struct {
    co_t     co; // must be inherited
    print_t *print_odd;
    print_t *print_even;
} print_example_t;

void print_example(print_example_t *co)
{
    // 协程开始
    co_begin(co);   // no line number need

    co->print_odd  = MALLOC(sizeof(*co->print_odd));
    co->print_even = MALLOC(sizeof(*co->print_even));
    print_init(co->print_odd, 1);
    print_init(co->print_even, 0);

    co_call(co, co->print_odd);
    co_call(co, co->print_even);

    FREE(co->print_odd);
    FREE(co->print_even);

    // 协程结束
    co_end(co);
}

print_example_t print_example_make()
{
    return (print_example_t){
        .co = CO(print_example),
    };
}

int main(void)
{
    generate_example();

    print_example_t co = print_example_make();
    co_run(&co);
    return 0;
}

// clang -std=c11 co_switch_goto_example.c -o /tmp/x && /tmp/x
