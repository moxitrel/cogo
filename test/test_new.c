//
// Created by m2 on 18/1/13.
//

#include "unity/unity.h"
#include "nstd/memory.h"

//#define HEAP(P) P = NEW(P)

//void *memory__new(size_t n, void **v)
//{
//    *v = malloc(n);
//    assert(*v);
//    return *v;
//}

void test_NEW(void)
{
    typedef struct T {
        int x;
        int y;
        int z;
    } *T;

    T HEAP(c) = (struct T){};

//    T x = (x = malloc(sizeof *x), assert(x), x); *x = (struct T){};
//    T (b = NULL) ;
    // Int NEW(a);
    // Int x = NEW(x);
    // Int HEAP(x);

}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_NEW);

    return UNITY_END();
}