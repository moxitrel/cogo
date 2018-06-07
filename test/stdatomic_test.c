// outfile="/tmp/test_stdatomic"; clang test_stdatomic.c ../lib/Unity/src/unity.c -o $outfile && $outfile

#include "../lib/Unity/src/unity.h"
#include <stdio.h>
#include <stdatomic.h>

void test_stdatomic_exist(void)
{
//    TEST_ASSERT_NOT_EQUAL(1, __STDC_NO_ATOMICS__)
}

void test_lock_free(void)
{
    printf("ATOMIC_BOOL_LOCK_FREE       : %d\n", ATOMIC_BOOL_LOCK_FREE);

    printf("ATOMIC_CHAR_LOCK_FREE       : %d\n", ATOMIC_CHAR_LOCK_FREE);
    printf("ATOMIC_CHAR16_T_LOCK_FREE   : %d\n", ATOMIC_CHAR16_T_LOCK_FREE);
    printf("ATOMIC_CHAR32_T_LOCK_FREE   : %d\n", ATOMIC_CHAR32_T_LOCK_FREE);
    printf("ATOMIC_WCHAR_T_LOCK_FREE    : %d\n", ATOMIC_WCHAR_T_LOCK_FREE);

    printf("ATOMIC_SHORT_LOCK_FREE      : %d\n", ATOMIC_SHORT_LOCK_FREE);
    printf("ATOMIC_INT_LOCK_FREE        : %d\n", ATOMIC_INT_LOCK_FREE);
    printf("ATOMIC_LONG_LOCK_FREE       : %d\n", ATOMIC_LONG_LOCK_FREE);
    printf("ATOMIC_LLONG_LOCK_FREE      : %d\n", ATOMIC_LLONG_LOCK_FREE);

    printf("ATOMIC_POINTER_LOCK_FREE    : %d\n", ATOMIC_POINTER_LOCK_FREE);
}


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_stdatomic_exist);
    RUN_TEST(test_lock_free);

    return UNITY_END();
}
