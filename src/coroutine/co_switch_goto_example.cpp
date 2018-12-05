#include "co_switch_goto.h"
#include <stdio.h>

//
// Used as generator
//

// define coroutine which print i, i+1, ..., j
// must inherit co_t
class PrintN : protected co_t
{
    int i;
    int j;

public:
    PrintN(int first, int last)
        : i(first)
        , j(last)
    {
    }

    // print i, i+1, ..., j
    void operator()()
    {
        // 协程开始
        co_begin(this, 31);

        // return i, i+1, ..., max
        for (; i <= j; i++) {
            printf("%d\n", i);
            co_return(this);
        }

        // 协程结束
        co_end(this);
    }
};

void Test_PrintN(void)
{
    // make a coroutine which print 1,2,3
    auto print13 = PrintN(1, 3);

    print13(); // print 1
    print13(); // print 2
    print13(); // print 3

    print13(); // do nothing, coroutine is finished
    print13(); // do nothing, coroutine is finished
}

int main()
{
    Test_PrintN();
}

// clang -std=c++17 co_switch_goto_example.cpp -o /tmp/x && /tmp/x