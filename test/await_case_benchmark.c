#include <time.h>
#include <stdio.h>
#include <stdint.h>

#define COGO_CASE
#include "../src/await.h"

typedef struct {
    await_t gen;
    intmax_t i;
} acase_t;

#define ACASE() ((acase_t){.gen = AWAIT(acase),})

void acase(acase_t *co)
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

void bench_acase(void)
{
    clock_t begin = clock();
    acase_t co = ACASE();
    await_run(&co);
    clock_t end = clock();
    printf("acase  : %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
