//
// Created by M2 on 2018-12-28.
//

#include "../src/coroutine.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

class Waiter: public co_t {
    co_block_t  &blk;
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
    Waiter(co_block_t &blk)
        : blk(blk)
    {}
};

class Counter: public co_t {
    co_block_t   &blk;
    Waiter          waiter = Waiter(blk);
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
    Counter(co_block_t &&blk)
        : blk(blk)
    {}
};


int main()
{
    // Output:
    //   0x7ffee62088e8 begin : Fri Dec 28 16:52:47 2018
    //   0x7ffee6208918 wakeup: Fri Dec 28 16:52:51 2018
    //   0x7ffee62088e8 end   : Fri Dec 28 16:52:54 2018
    Counter(co_block_t()).run();
}