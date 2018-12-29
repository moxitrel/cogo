//
// Created by M2 on 2018-12-28.
#undef __GNUC__
#include <stdio.h>
#include "../../src/coroutine/fun.hpp"

class print_t : public fun_t {
    char c;

    void operator()()
    {
        co_begin(16);

        for (c = 'a'; c < 'f'; c ++) {
            printf("%c\n", c);
            co_yield();
        }

        co_end();
    }
};


class loop3_t : public fun_t {
    print_t print;
    int i;

    void operator()()
    {
        co_begin(33);

        for (i = 0; i < 3; i ++) {
            co_await(print);
            printf("%d\n",i);
        }

        co_end();
    }
};


int main(void)
{
    loop3_t o;
    o.run();

    return 0;
}