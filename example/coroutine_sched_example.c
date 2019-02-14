//
// 1. Include header "coroutine.h"
//
#include "../src/cogo.h"
#include <stdio.h>

//
// 2. Define a struct which inherit co_t
//
typedef struct {
    // inherit co_t (as first field)
    co_t co;

    //
    // Define variables for coroutine function
    //
    int i;
} print06_t;

//
// 3. define coroutine function with the type "void (co_t *)"
//
void print06(print06_t *co)
{
    //
    // 4. Set coroutine begin.
    //
    co_begin(co);

    //
    // 5. User code
    //
    for (co->i = 0; co->i < 7; co->i++) {
        printf("%p:%d\n", co, co->i);
        co_yield(co);   // yield
    }

    //
    // 4. Set coroutine end.
    //
    co_end(co);
}

// 6. Define constructor
#define PRINT06() ((print06_t){.co = CO(print06),})


//
// 2. Define a struct inherit co_t
//
typedef struct {
    // inherit co_t
    co_t co;

    //
    // Define variables for coroutine function
    //
    print06_t print_co1;
    print06_t print_co2;
} coprint_t;

//
// 3. define coroutine function with the type "void (co_t *)"
//
void coroutine_example(coprint_t *co)
{
    //
    // 4. Set coroutine begin.
    //
    co_begin(co);

    //
    // 5. User code
    //
    co_sched(co, &co->print_co1);  // run print_co1 concurrently
    co_sched(co, &co->print_co2);  // run print_co2 concurrently

    //
    // 4. Set coroutine end
    //
    co_end(co);
}

// 6. Define constructor
#define COPRINT() ((coprint_t){     \
    .co = CO(coroutine_example),    \
    .print_co1 = PRINT06(),         \
    .print_co2 = PRINT06(),         \
})



int main()
{
    // Print:
    //  0x8f0:0
    //  0x8f0:1
    //  0x918:0
    //  0x8f0:2
    //  0x918:1
    //  0x8f0:3
    //  ...
    co_run(&COPRINT());
}
