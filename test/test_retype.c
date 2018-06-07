//
// Created by m2 on 18/1/15.
//

#include <stdint.h>
#include <stdio.h>
#include <stdatomic.h>

typedef union T {
    int   Int;
    float Float;
} T;

volatile float add_float(float x1, float x2)
{
    return x1 + x2;
}

//int add_int(int x1, int x2)
//{
//    return x1 + x2;
//}

int main(void)
{
    uintptr_t f = (uintptr_t)add_float;
    T v = ((volatile T (*)(float, float)) f)(1,1);
    printf("%f\n", v);
    return 0;
}