//
// 1. 包含头文件 "coroutine.h"
//
#include "../../src/coroutine/coroutine.h"
#include <stdio.h>

//
// 2. 自定义协程类，必须继承 co_t
//
// 协程打印 0x7ffee80:0, 0x7ffee80:1, ... 0x7ffee80:6
class PrintN : public co_t {
    //
    // 定义协程函数的 局部变量, 参数, 返回值, ...
    //
    int i;


    //
    // 3. 重载运算符operator(), 类型必须为 void()
    //
    void operator()()
    {
        //
        // 4. 标识 协程开始
        //
        co_begin(35);       // 35: 列出所有 co_return(), co_call(), co_sched() 所在的行号, 即 __LINE__ 的值
        // co_begin();      // 若开启GNUC扩展，可省略行号

        //
        // 5. 用户代码 (*** 无法使用局部变量, 用 类成员变量替代 ***)
        //            (不要使用 return)
        //
        for (i = 0; i < 7; i++) {
            printf("%p:%d\n", this, i);
            co_return();    // 返回，下次被调用，从此处开始执行
        }

     // co_call(coroutine); // await, 调用其他协程直至完成
     // co_sched(coroutine);// 添加协程到调度器，并发运行

        //
        // 4. 标识 协程结束
        //
        co_end();
    }
};

//
// 2. 自定义协程类，必须继承 co_t
//
// 创建 2 个并发运行的 PrintN 协程
class CoroutineExample : public co_t {
    //
    // 定义协程函数的 局部变量, 参数, 返回值, ...
    //
    PrintN coroutine1;
    PrintN coroutine2;

    //
    // 3. 重载运算符operator(), 类型必须为 void()
    //
    void operator()()
    {
        //
        // 4. 标识 协程开始
        //
        co_begin(73,74);        // 73,74: 列出所有 co_return(), co_call(), co_sched() 所在的行号, 即 __LINE__ 的值
     // co_begin();             // you can omit line numbers if enable GNUC extension

        //
        // 5. 用户代码
        //
        co_sched(coroutine1);   // 添加 coroutine1 到调度器中运行
        co_sched(coroutine2);   // 添加 coroutine2 到调度器中运行

        //
        // 4. 标识 协程结束
        //
        co_end();
    }
};

// 6. 使用
int main()
{
    // 运行直至结束
    // 输出:
    //  0x8f0:0
    //  0x8f0:1
    //  0x918:0
    //  0x8f0:2
    //  0x918:1
    //  0x8f0:3
    //  ...
    CoroutineExample().run();
}
