cmake_minimum_required(VERSION 3.13)
project(Cogo)
include(CTest)
include(CheckCCompilerFlag)

add_library(cogo co_st.c)
target_compile_features(cogo PUBLIC c_std_99)
check_c_compiler_flag(-Wno-unused-label HAS_FLAG_NO_UNUSED_LABEL)
check_c_compiler_flag(-Wno-gnu-label-as-value HAS_FLAG_NO_GNU_LABEL_AS_VALUE)
check_c_compiler_flag(-Wno-unreachable-code HAS_FLAG_NO_UNREACHABLE_CODE)
target_compile_options(cogo INTERFACE
    $<${HAS_FLAG_NO_UNUSED_LABEL}:-Wno-unused-label>
    $<${HAS_FLAG_NO_GNU_LABEL_AS_VALUE}:-Wno-gnu-label-as-value>
    $<${HAS_FLAG_NO_UNREACHABLE_CODE}:-Wno-unreachable-code>
    )

if (BUILD_TESTING AND (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME))
    include(GoogleTest)
    find_package(GTest REQUIRED)

    add_compile_options(-Wall -Wextra
        -Wno-c99-extensions
        -Wno-gnu-zero-variadic-macro-arguments

        -Wno-old-style-cast
        -Wno-zero-as-null-pointer-constant
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-padded
        -Wno-global-constructors        # google-test
        -Wno-missing-prototypes         # unit test
    )

    set(CMAKE_CXX_STANDARD 11)
    link_libraries(cogo GTest::GTest GTest::Main)

    # yield_case
    add_executable(test_yield_case yield_test.cpp)
    target_compile_definitions(test_yield_case PRIVATE COGO_CASE)
    gtest_discover_tests(test_yield_case)

    # yield_label_value
    add_executable(test_yield_label_value yield_test.cpp)
    target_compile_definitions(test_yield_label_value PRIVATE COGO_LABEL_VALUE)
    gtest_discover_tests(test_yield_label_value)

    # co
    add_executable(test_co co_test.cpp)
    gtest_discover_tests(test_co)

    # co_st
    add_executable(test_co_st co_st_test.cpp)
    gtest_discover_tests(test_co_st)
endif()
