#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include "../src/gen_line_no.hpp"

struct flineno_t : public gen_t {
    intmax_t i;

    void operator()()
    {
        co_begin(14,15,16,17,19,20,21);

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



void bench_flineno(void)
{
    clock_t begin = clock();
    flineno_t co;
    while (co.state() >= 0) {
        co();
    }
    clock_t end = clock();
    printf("flineno: %zd, %zd\n", end-begin, (end-begin) / CLOCKS_PER_SEC);
}
