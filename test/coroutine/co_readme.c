// 1. 包含头文件 "coroutine.h"
#undef __GNUC__
#include "../../src/coroutine/coroutine.h"
// 2. Define a struct inherit co_t
typedef struct {
    co_t co;    // inherit co_t (as first field)

    // Declare local variables, return values of coroutine function
    int value;
} nat_gen_t;

void nat_gen(nat_gen_t *co) // 3. Define coroutine function with the type "void (co_t *)"
{
    co_begin(co, 19);       // 4. Set Coroutine begin
 // co_begin(co);           // you can omit line numbers if enable GNUC extension

    // 5. User code
    for (co->value = 0; ; co->value++) {
        co_yield(co);      // yield
    }

    co_end(co);             // 4. Set coroutine end
}
// 6. Define constructor, init co_t member with CO()
#define NAT_GEN()   ((nat_gen_t){.co = CO(nat_gen),})

// 7. Use it
#include <stdio.h>
int main(void)
{
    // 初始化
    nat_gen_t ng = NAT_GEN();

    nat_gen(&ng);   // ng.value = 0
    printf("ng.value = %d\n", ng.value);

    nat_gen(&ng);   // ng.value = 1
    printf("ng.value = %d\n", ng.value);

    nat_gen(&ng);   // ng.value = 2
    printf("ng.value = %d\n", ng.value);

    return 0;
}