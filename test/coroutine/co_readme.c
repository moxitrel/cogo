// 1. 包含头文件 "coroutine.h"
#undef __GNUC__
#include "../../src/coroutine/coroutine.h"
// 2. 定义协程结构，必须继承 co_t
typedef struct {
    co_t co;    // 继承co_t (作为第1个字段)

    // 声明协程的 局部变量, 返回值, ...
    int value;
} nat_gen_t;    // 自然数生成器

// 3. 定义协程函数, 类型必须为 void (co_t *)
void nat_gen(nat_gen_t *co)
{
    co_begin(co, 20);   // 4. 标识 协程开始, 并列出所有 co_return(), co_call(), co_sched() 所在的行号, 即 __LINE__ 的值
 // co_begin(co);       // 若开启GNUC扩展，可省略行号

    // 5. 用户代码 (不要使用局部变量, 无法被恢复; 定义到 struct 中)
    for (co->value = 0; ; co->value++) {
        co_return(co);  // 返回，下次被调用，从此处开始执行
    }

    co_end(co);         // 4. 标识 协程结束
}
// 6. 定义构造器, 用 CO() 初始化 co_t 成员
#define NAT_GEN()   ((nat_gen_t){.co = CO(nat_gen),})

// 7. 使用
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