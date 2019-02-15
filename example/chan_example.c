//
// Created by M2 on 2019-01-30.
//
#include <assert.h>
#include "../src/co.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    co_t    co;
    chan_t *chan;
    time_t  now;
    void   *msg;
} waiter_t;

void waiter(waiter_t *co)
{
    co_begin(co);

    co_chan_read(co, co->chan, &co->msg);
    co->now = time(NULL);
    printf("%p wakeup: %s", co, ctime(&co->now));

    co_end(co);
}

// waiter_t WAITER(chan_t *);
#define WAITER(C) ((waiter_t){      \
    .co = CO(waiter),               \
    .chan = (C),                    \
})



typedef struct {
    co_t co;
    chan_t *chan;
    waiter_t waiter;
    time_t      now;
    intmax_t    i;
} counter_t;

void counter(counter_t *co)
{
    co_begin(co);

    co->now = time(NULL);
    printf("%p begin : %s", co, ctime(&co->now));

    co_start(co, &co->waiter);

    for (co->i = 0; co->i < ((intmax_t)1 << 31); co->i++) {
        if (co->i == ((intmax_t)1 << 30)) {
            co_chan_write(co, co->chan, NULL);
        }
    }

    co->now = time(NULL);
    printf("%p end   : %s", co, ctime(&co->now));

    co_end(co);
}

#define COUNTER(C)   ((counter_t){  \
    .co = CO(counter),              \
    .chan = (C),                    \
    .waiter = WAITER(C),            \
})

int main(void)
{
    chan_t c = CHAN();
    co_run(&COUNTER(&c));
}