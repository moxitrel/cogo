#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../src/gen_label_value.h"

typedef struct {
    gen_t gen;
    intmax_t i;
} flabel_t;

void flabel(flabel_t *co)
{
    intmax_t *i = &co->i;
    co_begin(co);

    for (*i = 0; *i < (intmax_t)1<<28; (*i)++) {
        co_yield(co);
        co_yield(co);
        co_yield(co);
        co_yield(co);
    }
    co_yield(co);
    co_return(co);
    co_yield(co);

    co_end(co);
}

void bench_flabel(void)
{
    clock_t begin = clock();
    flabel_t co = {};
    while (co_state(&co) >= 0) {
        flabel(&co);
    }
    clock_t end = clock();
    printf("flabel : %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
