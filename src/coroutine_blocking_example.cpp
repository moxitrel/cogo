//
// Created by M2 on 2018-12-28.
//
#undef __GNUC__
#include "co.hpp"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

class Nop : public co_t {
    void operator()()
    {
        co_begin(15);

        co_yield();

        co_end();
    }
};

class Waiter: public co_t {
    Nop nop;

    void operator()()
    {
        co_begin(28);

        co_await(nop);

        co_end();
    }
};

class Counter: public co_t {
    Waiter waiter;
    intmax_t        i;

    void operator()()
    {
        co_begin(43);

        for (i = 0; i < ((intmax_t)1 << 31); i++) {
            co_await(waiter);
        }

        co_end();
    }
};


int main()
{
    // Output:
    //   0x7ffee62088e8 begin : Fri Dec 28 16:52:47 2018
    //   0x7ffee6208918 wakeup: Fri Dec 28 16:52:51 2018
    //   0x7ffee62088e8 end   : Fri Dec 28 16:52:54 2018
    auto &&co = Counter();
//    co.run();
    sch_t().run(co);

}