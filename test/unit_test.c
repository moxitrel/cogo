/*** Unit Test

http://www.throwtheswitch.org/comparison-of-unit-test-frameworks
Unity      : pure C, rock for embed
Google Test: good option for mix C and C++
Check      : popular for posix


*** Make
outfile=$(mktemp); clang *_test.c ../lib/Unity/src/unity.c -o $outfile && $outfile

*** Doc

TEST_ASSERT                 (bool_exp)                  //assert if false
TEST_ASSERT_FLOAT_WITHIN    (delta, expected, actual)   //Float :
TEST_ASSERT_EQUAL_STRING    (expected, actual)          //String:
TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)     //String: no _ARRAY
TEST_ASSERT_EQUAL_MEMORY    (expected, actual, len)     //Memory: paddings are compared too!

*_MESSAGE: append _MESSAGE to any of the macros,
           assert with msg,
           e.g. TEST_ASSERT_MESSAGE(..., "msg")

*_ARRAY  : append _ARRAY to any of these macros,
           compare array
           e.g. TEST_ASSERT_EQUAL_MEMORY_ARRAY(..., elementCount)

TEST_IGNORE ()              //ignore test, return without fail
TEST_FAIL   ()              //mark test as fail

TEST_PROTECT()              //Setup, quit infinite loop
TEST_ABORT  ()              //Abort Test macro

// < = > !=
TEST_ASSERT_EQUAL       (expected , actual)
TEST_ASSERT_NOT_EQUAL   (expected , actual)
TEST_ASSERT_LESS_THAN   (threshold, actual)
TEST_ASSERT_GREATER_THAN(threshold, actual)


TEST_ASSERT_EQUAL_INT   (expected, actual)
TEST_ASSERT_EQUAL_INT8  (expected, actual)
TEST_ASSERT_EQUAL_INT16 (expected, actual)
TEST_ASSERT_EQUAL_INT32 (expected, actual)
TEST_ASSERT_EQUAL_INT64 (expected, actual)
TEST_ASSERT_EQUAL_UINT  (expected, actual)
TEST_ASSERT_EQUAL_UINT8 (expected, actual)
TEST_ASSERT_EQUAL_UINT16(expected, actual)
TEST_ASSERT_EQUAL_UINT32(expected, actual)
TEST_ASSERT_EQUAL_UINT64(expected, actual)
TEST_ASSERT_EQUAL_HEX   (expected, actual)
TEST_ASSERT_EQUAL_HEX8  (expected, actual)
TEST_ASSERT_EQUAL_HEX16 (expected, actual)
TEST_ASSERT_EQUAL_HEX32 (expected, actual)
TEST_ASSERT_EQUAL_HEX64 (expected, actual)

*/
#include "../lib/Unity/src/unity.h"

void test_???(void)
{

}


int main(void)
{
    UNITY_BEGIN();

    // RUN_TEST(func, linenum=__LINE__): Each Test is run within the macro
    RUN_TEST(test_???);
    RUN_TEST(...);
    ...

    return UNITY_END();
}
