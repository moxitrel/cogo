cmake_minimum_required(VERSION 3.13)
project(Cogo)

include(CTest)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

add_library(cogo INTERFACE)
target_sources(cogo INTERFACE co_st.c)
target_include_directories(cogo INTERFACE ${PROJECT_SOURCE_DIR})
target_compile_features(cogo INTERFACE c_std_99)
check_c_compiler_flag(-Wno-unused-label HAS_CFLAG_NO_UNUSED_LABEL)
check_c_compiler_flag(-Wno-gnu-label-as-value HAS_CFLAG_NO_GNU_LABEL_AS_VALUE)
check_c_compiler_flag(-Wno-c99-extensions HAS_CFLAG_NO_C99_EXTENSIONS)
check_cxx_compiler_flag(-Wno-old-style-cast HAS_CXXFLAG_NO_OLD_STYLE_CAST)
check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant HAS_CXXFLAG_NO_ZERO_AS_NULL_POINTER)
target_compile_options(cogo INTERFACE
    $<$<BOOL:${HAS_CFLAG_NO_UNUSED_LABEL}>:-Wno-unused-label>
    $<$<BOOL:${HAS_CFLAG_NO_GNU_LABEL_AS_VALUE}>:-Wno-gnu-label-as-value>
    $<$<BOOL:${HAS_CFLAG_NO_C99_EXTENSIONS}>:-Wno-c99-extensions>
    $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${HAS_CXXFLAG_NO_OLD_STYLE_CAST}>>:-Wno-old-style-cast>
    $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<BOOL:${HAS_CXXFLAG_NO_ZERO_AS_NULL_POINTER}>>:-Wno-zero-as-null-pointer-constant>
    )

if (BUILD_TESTING AND (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME))
    include(GoogleTest)
    find_package(GTest REQUIRED)

    check_cxx_compiler_flag(-Weverything HAS_FLAG_EVERYTHING)
    check_cxx_compiler_flag(-Wno-c++98-compat HAS_CXXFLAG_NO_CPP98_COMPAT)
    check_cxx_compiler_flag(-Wno-c++98-compat-pedantic HAS_CXXFLAG_NO_CPP98_COMPAT_PEDANTIC)
    check_cxx_compiler_flag(-Wno-gnu-zero-variadic-macro-arguments HAS_FLAG_NO_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS)
    check_cxx_compiler_flag(-Wno-global-constructors HAS_FLAG_NO_GLOBAL_CONSTRUCTORS)
    check_cxx_compiler_flag(-Wno-padded HAS_FLAG_NO_PADDED)
    add_compile_options(
        $<$<BOOL:${HAS_FLAG_EVERYTHING}>:-Weverything>
        $<$<BOOL:${HAS_CXXFLAG_NO_CPP98_COMPAT}>:-Wno-c++98-compat>
        $<$<BOOL:${HAS_CXXFLAG_NO_CPP98_COMPAT_PEDANTIC}>:-Wno-c++98-compat-pedantic>
        $<$<BOOL:${HAS_FLAG_NO_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS}>:-Wno-gnu-zero-variadic-macro-arguments>
        $<$<BOOL:${HAS_FLAG_NO_GLOBAL_CONSTRUCTORS}>:-Wno-global-constructors> # google-test

        $<$<BOOL:${HAS_FLAG_NO_PADDED}>:-Wno-padded>
        -Wno-static-in-inline
    )

    set(CMAKE_CXX_STANDARD 11)
    link_libraries(cogo GTest::GTest GTest::Main)

    # yield_case
    add_executable(cogo_yield_case_test cogo_yield_test.cpp)
    target_compile_definitions(cogo_yield_case_test PRIVATE COGO_CASE)
    target_compile_options(cogo_yield_case_test PRIVATE -Wno-unreachable-code)
    gtest_discover_tests(cogo_yield_case_test)

    # yield_label_value
    add_executable(cogo_yield_label_value_test cogo_yield_test.cpp)
    target_compile_definitions(cogo_yield_label_value_test PRIVATE COGO_LABEL_VALUE)
    target_compile_options(cogo_yield_label_value_test PRIVATE -Wno-unreachable-code)
    gtest_discover_tests(cogo_yield_label_value_test)

    # co
    add_executable(cogo_co_test cogo_co_test.cpp)
    gtest_discover_tests(cogo_co_test)

    # co_st
    add_executable(co_st_test co_st_test.cpp)
    gtest_discover_tests(co_st_test)
endif()
