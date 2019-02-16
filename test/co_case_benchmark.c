#include <time.h>
#include <stdio.h>
#include <stdint.h>

#define COGO_CASE
#include "../src/co.h"

typedef struct {
    co_t co_t;
    intmax_t i;
} ccase_t;

#define CCASE() ((ccase_t){.co_t = CO(ccase),})

void ccase(ccase_t *co)
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
    co_yield(co);
    co_yield(co);

    co_end(co);
}

void bench_ccase(void)
{
    clock_t begin = clock();
    ccase_t co = CCASE();
    co_run(&co);
    clock_t end = clock();
    printf("ccase  : %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
