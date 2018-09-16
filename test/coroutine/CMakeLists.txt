# One CMakeLists.txt per target
#
# command (args ...)
#


add_executable(test_coroutine
        co_goto_test.c
        ../../lib/Unity/src/unity.c)

add_test(NAME test_coroutine
        COMMAND test_coroutine)