//
// 1. 包含头文件
//
#include "../../src/coroutine/coroutine.h"
#include <stdio.h>

//
// 2. 自定义协程类，必须继承 co_t
//
class PrintNat : public co_t {
    //
    // 定义 局部变量, 返回值, ...
    //
    int i;


    //
    // 3. 重载运算符operator(), 类型必须为 void()
    //
    void operator()()
    {
        //
        // 4. 标识 协程开始 (必须)
        //
        co_begin(34,38,...);    // 21,24,...: 列出所有 co_return(), co_call(), co_sched() 所在的行号, 即 __LINE__ 的值
        // co_begin();             // 若开启GNUC扩展，可省略行号

        //
        // 5. 用户代码 (*** 无法使用局部变量, 用 类成员变量替代 ***)
        //            (不要使用 return)
        //
        for (i = 0; i < 9; i++) {
            printf("%d\n", i);
            co_return();    // 返回，下次被调用，从此处开始执行
        }

        printf("%d\n", i);
        co_return();        // 返回，下次被调用，从此处开始执行


        //
        // 4. 标识 协程结束 (必须)
        //
        co_end();
    }
};

//
// 6. 使用
//
void example()
{
    PrintNat printNat;

    // Run until finish.
    // Print:
    //  0
    //  1
    //  2
    //  ...
    printNat.run();
}

int main()
{
    example();
}