//
// Created by M2 on 2018-12-28.
#undef __GNUC__
#include <stdio.h>
#include "../src/await.hpp"

class print_t : public await_t {
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


class loop3_t : public await_t {
    print_t print;
    int i;

    void operator()()
    {
        co_begin(34);

        for (i = 0; i < 3; i ++) {
            print = print_t();
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