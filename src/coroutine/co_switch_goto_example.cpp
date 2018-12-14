#include "co_switch_goto.hpp"
#include <stdio.h>

//
// Used as generator
//

// define coroutine which print i, i+1, ..., j
// must inherit co_t
class PrintN : public co_t
{
    int i;
    int j;

public:
    PrintN(int first, int last)
        : i(first)
        , j(last)
        , co_t()
    {
    }

    // print i, i+1, ..., j
    void operator()() override
    {
        // 协程开始
        co_begin(31);

        for (; i <= j; i++) {
            printf("%d\n", i);
            co_return();    // 协程返回, 下次调用从此处开始执行
        }

        // 协程结束
        co_end();
    }
};

void Test_PrintN(void)
{
    // make a generator which print 1,2,3
    auto print13 = PrintN(1, 3);

    print13(); // print 1
    print13(); // print 2
    print13(); // print 3

    print13(); // do nothing, coroutine is finished
    print13(); // do nothing, coroutine is finished
    printf("\n");

    // print 11, 12, 13
    co_run(PrintN(11,13));
}

int main()
{
    Test_PrintN();
}

// clang -std=c++17 co_switch_goto_example.cpp -o /tmp/x && /tmp/x
