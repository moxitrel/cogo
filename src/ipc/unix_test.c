//
// Created by m2 on 18/1/22.
//

#include "../test/unity/unity.h"
#include "unix.h"
#include "../memory.h"
#include <stdio.h>

void test_UnixIpcAddr_path_null(void);
void test_UnixIpcAddr_path_zeroLength(void);
void test_UnixIpcAddr_path_normal(void);
void test_UnixIpcAddr_path_tooLong(void);
void test_UnixIpc_With(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_UnixIpcAddr_path_null);
    RUN_TEST(test_UnixIpcAddr_path_zeroLength);
    RUN_TEST(test_UnixIpcAddr_path_normal);
    RUN_TEST(test_UnixIpcAddr_path_tooLong);
    RUN_TEST(test_UnixIpc_With);

    return UNITY_END();
}



void test_UnixIpcAddr_path_null(void)
{
    UnixIpcAddr x = UnixIpcAddr_New(NULL);
    TEST_ASSERT_NULL(x);
    UnixIpcAddr_Gc(&x);
}

void test_UnixIpcAddr_path_zeroLength(void)
{
    UnixIpcAddr x = UnixIpcAddr_New("");
    TEST_ASSERT_NOT_NULL(x);
    UnixIpcAddr_Gc(&x);
}

void test_UnixIpcAddr_path_normal(void)
{
    TEST_FAIL_MESSAGE("havn't implemented!");
}

void test_UnixIpcAddr_path_tooLong(void)
{
    TEST_FAIL_MESSAGE("havn't implemented!");
}

int Return0(UnixIpc o)
{
    return 0;
}
void test_UnixIpc_With(void)
{
    const char *const LOCAL_PATH = tmpnam(NULL);
    if (!LOCAL_PATH) {
        TEST_IGNORE_MESSAGE("Can't make a non-NULL LOCAL_PATH");
    }
    int err = UnixIpc_With(LOCAL_PATH, "", Return0);
    TEST_ASSERT(err == 0);
}