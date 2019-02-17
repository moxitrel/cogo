cmake_minimum_required(VERSION 3.13)

project(cogo
        DESCRIPTION "C/C++ stackless coroutine library")

enable_testing()

add_subdirectory(test)
add_subdirectory(example EXCLUDE_FROM_ALL)
