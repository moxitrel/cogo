#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../src/gen_case.h"

typedef struct {
    gen_t gen_t;
    intmax_t i;
} fcase_t;

void fcase(fcase_t *co)
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

void bench_fcase(void)
{
    clock_t begin = clock();
    fcase_t co = {};
    while (co_state(&co) >= 0) {
        fcase(&co);
    }
    clock_t end = clock();
    printf("fcase  : %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
