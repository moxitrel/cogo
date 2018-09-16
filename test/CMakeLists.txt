# One CMakeLists.txt per target
#
# command (args ...)
#

add_subdirectory(coroutine)

add_executable(test_stack
        stack_test.c
        ../lib/Unity/src/unity.c)

add_test(NAME test_stack
        COMMAND test_stack)