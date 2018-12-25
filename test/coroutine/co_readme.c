//
// 1. 包含头文件 "coroutine.h"
//
#include "../../src/coroutine/coroutine.h"

#include <stdio.h>

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
    int i;
} print_nat_t;

//
// 3. 定义协程函数, 类型必须为 void (co_t *)
//
void print_nat(print_nat_t *co)
{
    //
    // 4. 标识 协程开始 (必须)
    //
    co_begin(co, 21,24,...);    // 21,24,...: 列出所有 co_return(), co_call(), co_sched() 所在的行号, 即 __LINE__ 的值
    // co_begin(co);               // 若开启GNUC扩展，可省略行号

    //
    // 5. 用户代码 (*** 不要使用局部变量, 无法被恢复; 定义到 struct字段 中 ***)
    //            (不要使用 return)
    //
    for (co->i = 0; co->i < 9; co->i++) {
        printf("%d\n", co->i);
        co_return(co);          // 返回，下次被调用，从此处开始执行
    }

    printf("%d\n", co->i);
    co_return(co);              // 返回，下次被调用，从此处开始执行


    //
    // 4. 标识 协程结束 (必须)
    //
    co_end(co);
}

//
// 6. 使用
//
void example()
{
    // 7. 初始化 协程结构
    print_nat_t printNat = {
            .co = CO(print_nat),    // 初始化 co_t
            //  .i = ???,               // 忽略 i 的初始化
    };

    // Run until finish.
    // Print:
    //  0
    //  1
    //  2
    //  ...
    co_run((co_t *)&printNat);
}

int main(void)
{
    example();
}