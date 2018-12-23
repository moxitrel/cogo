#include "../../src/memory.h"
#include "../../src/coroutine/gen.hpp"
#include <stdio.h>

// 必须继承 gen_t
struct T : public gen_t {
    // 定义 局部变量, 返回值, ...
    int i;

    // 定义协程函数
    void operator()()
    {
        // 协程开始
        co_begin(20, 24);

        printf("%d\n", i);

        for (i = 0; i < 3; i++) {
            printf("%d\n", i);
            co_return();
        }

        printf("%d\n", i);
        co_return();

        // 协程结束
        co_end();
    }
};

int main()
{
    T gen;

    // gen.state(): 获取运行状态
    //    0: 准备就绪
    //   >0: 正在运行
    //   <0: 结束
    while (gen.state() >= 0) {
        gen();
    }

    return 0;
}
