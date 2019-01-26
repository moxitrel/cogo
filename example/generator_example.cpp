#include "../src/gen.hpp"
#include <stdio.h>

struct NatGen : public gen_t {
    int i;

    void operator()()
    {
        co_begin(12);

        for (i = 0;;i++) {
            co_yield();
        }

        co_end();
    }
};

int main()
{
    NatGen co = {};
    for (int i = 0; i < 9; i++) {
        co();
        printf("%d\n", co.i);
    }
}
