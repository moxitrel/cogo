// outfile=$(mktemp); clang co_goto_test.c ../../lib/Unity/src/unity.c -o
// $outfile && $outfile

#include "../../lib/Unity/src/unity.h"
#include "../../src/coroutine/co_goto.h"

// gen_0_13: 生成器, 返回 0, 1, 2, ... 13
//
// DEFINE_CO(gen_0_13, int value, int i) 等价于:
//
//    typedef struct {
//        CoroutineContext;
//
//        int value;    // 保存返回值
//        int i;        // 局部变量
//    } gen_0_13_ctx_t;
//
// void gen_0_13(ctx_t *ctx)
//
CO_FUN(gen_0_13, int value, int i)
{
    // BEGIN() 之前的代码，每次调用都会被执行
    int *value = CO_ARG(gen_0_13, value); // & ((gen_0_13_ctx_t *)ctx) -> value
    int *i     = CO_ARG(gen_0_13, i);     // & ((gen_0_13_ctx_t *)ctx) -> i

    CO_BEGIN(32, 35, 36, 37, 38); // 协程开始
                                  // 33,37, ...: YIELD() 所在的行号; 若不正确, 则会编译失败
                                  // (e.g. use of undeclared label 'yield_32')

    // 返回 0, 1, ... 9
    for (; *i < 10; (*i)++) {
        YIELD(*value = *i);     //存储返回值, 并返回
    }

    YIELD(*value = (*i)++); // value=10, i=11
    YIELD(*value = (*i)++); // value=11, i=12
    YIELD(*value = (*i)++); // value=12, i=13
    YIELD(*value = (*i)++); // value=13, i=14

    CO_END(); //协程结束
}

//void test_gen_0_13(void)
//{
//    gen_0_13_ctx_t ctx = {};
//
//    for (int i = 0; i < 14; i++) {
//        gen_0_13((ctx_t *)&ctx);
//        TEST_ASSERT(ctx.value == i);
//    }
//
//    TEST_ASSERT(ctx.yield > 0); // 此时，协程未运行完毕，即END()未执行
//    gen_0_13((ctx_t *)&ctx);    // 运行END()
//    TEST_ASSERT(ctx.yield < 0); // 协程已运行结束
//}

void test_call(void)
{
    CO_RUN(gen_0_13, 0, 0);
}

int main(void)
{
    UNITY_BEGIN();

//    RUN_TEST(test_gen_0_13);
    RUN_TEST(test_call);

    return UNITY_END();
}
