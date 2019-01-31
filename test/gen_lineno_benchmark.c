#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../src/gen_line_no.h"

typedef struct {
    gen_t gen;
    intmax_t i;
} flineno_t;

void flineno(flineno_t *co)
{
    intmax_t *i = &co->i;
    co_begin(co,17,18,19,20,22,23,24);

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

void bench_flineno(void)
{
    clock_t begin = clock();
    flineno_t co = {};
    while (co_state(&co) >= 0) {
        flineno(&co);
    }
    clock_t end = clock();
    printf("flineno: %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
