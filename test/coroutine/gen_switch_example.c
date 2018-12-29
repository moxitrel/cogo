// 1. include header
#include "../../src/coroutine/gen_switch.h"

// 2. inherit co_t
typedef struct {
    gen_t co;   // put in first

    //
    // declare local variables, parameters, return values of coroutine function
    //
    int i;
//    ...
} co_fun_t;

// 3. define a function with type "void (T *)"
void co_fun(co_fun_t *co)
{
    //
    // before co_begin(), codes run every time when invoked
    //

    // e.g. alias
    int *i = &co->i;

    // 4. set coroutine begin
    co_begin(co, 30); // 30: list line numbers of co_yield(), co_return(), i.e. __LINE__

    // 5. user codes: (don't use local variables)
    for (*i = 0 ; *i < 9; (*i) ++) {
        co_yield(co);   // yield
    }

    // 4. set coroutine end
    co_end(co);

    //
    // after co_end(), codes run every time before return
    //
}

// 6. define constructor
#define CO_FUN(...)  ((co_fun_t){})

//
// example
//
int main(void)
{
    co_fun_t co = CO_FUN();

    co_fun(&co);    // co->i = 0
    co_fun(&co);    // co->i = 1
    co_fun(&co);    // co->i = 2
}