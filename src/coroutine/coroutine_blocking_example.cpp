//
// Created by M2 on 2018-12-28.
//

#include "co.hpp"
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

class Waiter: public co_t {
    co_blocking_t  &blk;
    time_t          now;

    void operator()()
    {
        co_begin();

        co_wait(blk);   // block until notified (co_broadcast())
        now = time(nullptr);
        printf("%p wakeup: %s", this, ctime(&now));

        co_end();
    }

public:
    Waiter(co_blocking_t &blk)
        : blk(blk)
    {}
};

class Counter: public co_t {
    co_blocking_t   &blk;
    Waiter          waiter;
    time_t          now;
    intmax_t        i;

    void operator()()
    {
        co_begin();
        now = time(nullptr);
        printf("%p begin : %s", this, ctime(&now));

        co_sched(waiter);

        for (i = 0; i < ((intmax_t)1 << 31); i++) {
            if (i == ((intmax_t)1 << 30)) {
                co_broadcast(blk);  // wake up coroutines blocked by blk
            }
        }

        now = time(nullptr);
        printf("%p end   : %s", this, ctime(&now));
        co_end();
    }

public:
    Counter(co_blocking_t &&blk)
        : blk(blk)
        , waiter(blk)
    {}
};


int main()
{
    Counter(co_blocking_t())
        .run();
}