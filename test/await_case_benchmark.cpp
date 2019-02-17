#include <time.h>
#include <stdio.h>
#include <stdint.h>

#define COGO_CASE
#include "../src/await.hpp"

class acase_t : public await_t {
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



void bench_acase(void)
{
    clock_t begin = clock();
    acase_t().run();
    clock_t end = clock();
    printf("acase  : %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
