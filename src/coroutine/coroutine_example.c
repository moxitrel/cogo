//
// 1. Include header "coroutine.h"
//
#include "coroutine.h"
#include <stdio.h>

//
// 2. Define a struct which inherit co_t
//
typedef struct {
    // inherit co_t (as first field)
    co_t co;

    //
    // Define local variables, parameters, return values of coroutine function
    //
    int i;
} co_print_t;

//
// 3. define coroutine function which has the type "void (co_t *)"
//
// A coroutine print 0x7ffee80:0, 0x7ffee80:1, ... 0x7ffee80:6
void co_print(co_print_t *co)
{
    //
    // 4. Set coroutine begin. 37: line number of co_yield(), co_await(), co_sched()
    //
    co_begin(co, 37);
 // co_begin(co);       // you can omit line numbers if enable GNUC extension

    //
    // 5. User codes
    //
    for (co->i = 0; co->i < 7; co->i++) {
        printf("%p:%d\n", co, co->i);
        co_yield(co);  // yield
    }

    //
    // 4. Set coroutine end
    //
    co_end(co);
}
// 6. Define coroutine constructor
#define CO_PRINT() ((co_print_t){.co = CO(co_print),})


//
// 2. Define a struct which inherit co_t
//
typedef struct {
    // inherit co_t (as first field)
    co_t co;

    //
    // Define local variables, parameters, return values of coroutine function
    //
    co_print_t coroutine1;
    co_print_t coroutine2;
} coroutine_example_t;

//
// 3. define coroutine function which has the type "void (co_t *)"
//
// A coroutine create two co_print coroutine which run concurrently.
void coroutine_example(coroutine_example_t *co) // define coroutine function
{
    //
    // 4. Set coroutine begin. 37,38: line number of co_yield(), co_await(), co_sched()
    //
    co_begin(co,78,79);
 // co_begin(co);                   // you can omit line numbers if enable GNUC extension

    //
    // 5. User codes
    //
    co_sched(co, &co->coroutine1);  // run coroutine1 concurrently
    co_sched(co, &co->coroutine2);  // run coroutine2 concurrently

    //
    // 4. Set coroutine end
    //
    co_end(co);
}
// 6. Define coroutine constructor
#define COROUTINE_EXAMPLE() ((coroutine_example_t){ \
    .co = CO(coroutine_example),                    \
    .coroutine1 = CO_PRINT(),                       \
    .coroutine2 = CO_PRINT(),                       \
})

//
// 7. Use it
//
int main()
{
    // Run until finish all coroutines.
    // Print:
    //  0x8f0:0
    //  0x8f0:1
    //  0x918:0
    //  0x8f0:2
    //  0x918:1
    //  0x8f0:3
    //  ...
    coroutine_example_t co = COROUTINE_EXAMPLE();
    co_run((co_t *)&co);
}

// clang -std=c17 coroutine_example.c -o /tmp/coroutine_example && /tmp/coroutine_example
