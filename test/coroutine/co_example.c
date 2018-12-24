
#include "../../src/coroutine/co.h"               // 1. include co.hpp
#include <stdio.h>

typedef struct {
    co_t co;
    const int id;
    int i;
} co_print_t;

void co_print(co_print_t *co)
{
    co_begin(co, 17);

    for (co->i = 0; co->i < 5; co->i++) {
        printf("%d:%d\n", co->id, co->i);
        co_return(co);
    }

    co_end(co);
}

#define CO_PRINT(ID) ((co_print_t){.co = CO(co_print), .id = (ID),})

typedef struct {
    co_t co;
    co_print_t p1;
    co_print_t p2;
} co_example_t;

void co_example(co_example_t *co)
{
    co_begin(co,35,36);

    co_sched(co, &co->p1);   // schedule to run coroutine1 concurrently
    co_sched(co, &co->p2);   // schedule to run coroutine2 concurrently

    co_end(co);
}

int main()
{
    // Run until finish all coroutines.
    // Print:
    //  1:0
    //  1:1
    //  2:0
    //  1:2
    //  2:1
    //  1:3
    //  ...
    co_example_t co = {
        .co = CO(co_example),
        .p1 = CO_PRINT(1),
        .p2 = CO_PRINT(2),
    };
    co_run(&co);
}

// clang -std=c++17 co_example.cpp -lstdc++ -o /tmp/CoroutineExample1 && /tmp/CoroutineExample1
