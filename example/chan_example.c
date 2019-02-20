//
// Created by M2 on 2019-01-30.
//
#include <assert.h>
#include "../src/co.h"
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    co_t    co_t;
    chanptr_t chan;
    void   *msg;
} waiter_t;

void waiter(waiter_t *co)
{
    co_begin(co);

    co_chan_read(co, co->chan, &co->msg);
    printf("%p wakeup: %s", co, ctime(&(time_t){time(NULL)}));

    co_end(co);
}

// waiter_t WAITER(chanptr_t *);
#define WAITER(C) ((waiter_t){      \
    .co_t = CO(waiter),             \
    .chan = (C),                    \
})



typedef struct {
    co_t co_t;
    chanptr_t chan;
    waiter_t waiter;
    intmax_t    i;
} counter_t;

void counter(counter_t *co)
{
    co_begin(co);

    printf("%p begin : %s", co, ctime(&(time_t){time(NULL)}));
    co_start(co, &co->waiter);

    for (co->i = 0; co->i < ((intmax_t)1 << 31); co->i++) {
        if (co->i == ((intmax_t)1 << 30)) {
            co_chan_write(co, co->chan, NULL);
        }
    }

    printf("%p end   : %s", co, ctime(&(time_t){time(NULL)}));
    co_end(co);
}

#define COUNTER(C)   ((counter_t){  \
    .co_t = CO(counter),            \
    .chan = (C),                    \
    .waiter = WAITER(C),            \
})

int main(void)
{
    chanptr_t c = CHANPTR(0);
    co_run(&COUNTER(c));
}