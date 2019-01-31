#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../src/gen_case.hpp"

struct fcase_t : public gen_t {
    intmax_t i;

    void operator()()
    {
        co_begin();

        for (i = 0; i < (intmax_t)1<<28; i++) {
            co_yield();
            co_yield();
            co_yield();
            co_yield();
        }
        co_yield();
        co_yield();
        co_yield();

        co_end();
    }
};



void bench_fcase(void)
{
    clock_t begin = clock();
    fcase_t co;
    while (co.state() >= 0) {
        co();
    }
    clock_t end = clock();
    printf("fcase  : %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
