cmake_minimum_required(VERSION 3.13)
project(Cogo)
include(CTest)
include(CheckCCompilerFlag)

add_library(cogo co_st.c)
target_compile_features(cogo PUBLIC c_std_99)
target_include_directories(cogo INTERFACE ${PROJECT_SOURCE_DIR})
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
    add_executable(yield_case_test yield_test.cpp)
    target_compile_definitions(yield_case_test PRIVATE COGO_CASE)
    gtest_discover_tests(yield_case_test)

    # yield_label_value
    add_executable(yield_label_value_test yield_test.cpp)
    target_compile_definitions(yield_label_value_test PRIVATE COGO_LABEL_VALUE)
    gtest_discover_tests(yield_label_value_test)

    # co
    add_executable(co_test co_test.cpp)
    gtest_discover_tests(co_test)

    # co_st
    add_executable(co_st_test co_st_test.cpp)
    gtest_discover_tests(co_st_test)
endif()
