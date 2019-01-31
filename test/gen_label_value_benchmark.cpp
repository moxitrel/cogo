#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../src/gen_label_value.hpp"

struct flabel_t : public gen_t {
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



void bench_flabel(void)
{
    clock_t begin = clock();
    flabel_t co;
    while (co.state() >= 0) {
        co();
    }
    clock_t end = clock();
    printf("flabel : %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
