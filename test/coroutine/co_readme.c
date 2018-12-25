//
// 1. 包含头文件 "coroutine.h"
//
#include "../../src/coroutine/coroutine.h"

//
// 2. 自定义协程结构，必须继承 co_t
//
typedef struct {
    //
    // 继承co_t (作为第1个字段)
    //
    co_t co;

    //
    // 定义 局部变量, 返回值, ...
    //
    int value;
} nat_gen_t;

//
// 3. 定义协程函数, 类型必须为 void (co_t *)
//
// 自然数生成器
void nat_gen(nat_gen_t *co)
{
    //
    // 4. 标识 协程开始 (必须)
    //
    co_begin(co, 38);           // 38, ...: 列出所有 co_return(), co_call(), co_sched() 所在的行号, 即 __LINE__ 的值
 // co_begin(co);               // 若开启GNUC扩展，可省略行号

    //
    // 5. 用户代码 (*** 不要使用局部变量, 无法被恢复; 定义到 struct字段 中 ***)
    //            (不要使用 return)
    //
    for (co->value = 0; ; co->value++) {
        co_return(co);          // 返回，下次被调用，从此处开始执行
    }

 // co_call(co, coroutine);     // await, 调用其他协程直至完成
 // co_sched(co, coroutine);    // 添加协程到调度器，并发运行

    //
    // 4. 标识 协程结束 (必须)
    //
    co_end(co);
}
// 6. 定义构造器, 用 CO() 初始化 co 成员
#define NAT_GEN()   ((nat_gen_t){.co = CO(nat_gen),})

// 7. 使用
#include <stdio.h>
int main(void)
{
    // 7. 初始化 协程结构
    nat_gen_t n1 = NAT_GEN();

    nat_gen(&n1);   // n1.value = 0
    printf("n1.value = %d\n", n1.value);

    nat_gen(&n1);   // n1.value = 1
    printf("n1.value = %d\n", n1.value);

    nat_gen(&n1);   // n1.value = 2
    printf("n1.value = %d\n", n1.value);

    return 0;
}