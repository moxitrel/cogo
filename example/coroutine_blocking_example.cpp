//
// Created by M2 on 2018-12-28.
//

#include "../src/cogo.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

class Waiter: public co_t {
    chan_t &chan;
    time_t  now;
    void   *msg;

    void operator()()
    {
        co_begin();

        co_chan_read(chan,&msg);
        now = time(nullptr);
        printf("%p wakeup: %s", this, ctime(&now));

        co_end();
    }

public:
    Waiter(chan_t &chan)
        : chan(chan)
    {}
};

class Counter: public co_t {
    chan_t     &chan;
    Waiter      waiter = Waiter(chan);
    time_t      now;
    intmax_t    i;

    void operator()()
    {
        co_begin();
        now = time(nullptr);
        printf("%p begin : %s", this, ctime(&now));

        co_sched(waiter);

        for (i = 0; i < ((intmax_t)1 << 31); i++) {
            if (i == ((intmax_t)1 << 30)) {
                co_broadcast(chan);  // wake up coroutines blocked by chan
            }
        }

        now = time(nullptr);
        printf("%p end   : %s", this, ctime(&now));
        co_end();
    }

public:
    Counter(chan_t &&chan)
        : chan(chan)
    {}
};


int main()
{
    // Output:
    //   0x7ffee62088e8 begin : Fri Dec 28 16:52:47 2018
    //   0x7ffee6208918 wakeup: Fri Dec 28 16:52:51 2018
    //   0x7ffee62088e8 end   : Fri Dec 28 16:52:54 2018
    Counter(chan_t()).run();
}