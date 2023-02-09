cmake_minimum_required(VERSION 3.13)
project(cogo)

include(CTest)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

add_library(cogo
    cogo_co.c
    co_st.c
)
target_include_directories(cogo PUBLIC include)
target_compile_features(cogo PUBLIC c_std_99)

target_compile_options(cogo PRIVATE
    $<$<BOOL:${W_EVERYTHING}>:-Weverything>
    -Wno-error
)

if (BUILD_TESTING AND (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME))
    # find_package(GTest REQUIRED)
    # include(GoogleTest)
    include(cmake/unity.cmake)

    set(CMAKE_C_COMPILER    clang++)

    check_cxx_compiler_flag(-Weverything W_EVERYTHING)
    check_cxx_compiler_flag(-Wno-extra-semi-stmt WNO_EXTRA_SEMI_STMT)
    check_cxx_compiler_flag(-Wno-missing-prototypes WNO_MISSING_PROTOTYPES)
    check_cxx_compiler_flag(-Wno-unreachable-code WNO_UNREACHABLE_CODE)
    check_cxx_compiler_flag(-Wno-padded WNO_PADDED)
    check_cxx_compiler_flag(-Wno-missing-field-initializers WNO_MISSING_FIELD_INITIALIZERS)
    check_cxx_compiler_flag(-Wno-old-style-cast WNO_OLD_STYLE_CAST)
    # check_cxx_compiler_flag(-Wno-global-constructors WNO_GLOBAL_CONSTRUCTORS)
    # check_cxx_compiler_flag(-Wno-c++98-compat-pedantic WNO_CPP98_COMPAT_PEDANTIC)
    # check_cxx_compiler_flag(-Wno-gnu-zero-variadic-macro-arguments HAS_FLAG_NO_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS)
    # check_cxx_compiler_flag(-Wno-static-in-inline HAS_FLAG_NO_STATIC_IN_INLINE)

    # check_cxx_compiler_flag(-Wno-zero-as-null-pointer-constant WNO_ZERO_AS_NULL_POINTER_CONSTANT)
    add_compile_options(
        $<$<BOOL:${W_EVERYTHING}>:-Weverything>
        # $<$<BOOL:${WNO_EXTRA_SEMI_STMT}>:-Wno-extra-semi-stmt>
        $<$<BOOL:${WNO_PADDED}>:-Wno-padded>
        $<$<BOOL:${WNO_MISSING_PROTOTYPES}>:-Wno-missing-prototypes>
        $<$<BOOL:${WNO_UNREACHABLE_CODE}>:-Wno-unreachable-code>
        $<$<BOOL:${WNO_MISSING_FIELD_INITIALIZERS}>:-Wno-missing-field-initializers>
        $<$<BOOL:${WNO_OLD_STYLE_CAST}>:-Wno-old-style-cast>
        # $<$<BOOL:${WNO_GLOBAL_CONSTRUCTORS}>:-Wno-global-constructors> # google-test
        # $<$<BOOL:${WNO_CPP98_COMPAT_PEDANTIC}>:-Wno-c++98-compat-pedantic>

        # $<$<BOOL:${HAS_FLAG_NO_GNU_ZERO_VARIADIC_MACRO_ARGUMENTS}>:-Wno-gnu-zero-variadic-macro-arguments>
        # $<$<BOOL:${HAS_FLAG_NO_STATIC_IN_INLINE}>:-Wno-static-in-inline>
        # $<$<BOOL:${WNO_ZERO_AS_NULL_POINTER_CONSTANT}>:-Wno-zero-as-null-pointer-constant>
        #
        # -Wno-useless-cast
        -Wno-c++98-compat-pedantic
        -Wno-c++20-designator
        -Wno-c99-extensions
        -Wno-zero-as-null-pointer-constant
        -Wno-error
    )

    link_libraries(cogo)

    # cogo_yield: case
    add_executable(test_yield_case cogo_yield_test.c)
    target_compile_definitions(test_yield_case PRIVATE COGO_YIELD_CASE)
    add_test(NAME test_yield_case COMMAND test_yield_case)
    # cogo_yield: label value
    add_executable(test_yield_label_value cogo_yield_test.c)
    target_compile_definitions(test_yield_label_value PRIVATE COGO_YIELD_LABEL_VALUE)
    add_test(NAME test_yield_label_value COMMAND test_yield_label_value)

    # cogo_co
    add_executable(test_cogo_co cogo_co_test.c)
    add_test(NAME test_cogo_co COMMAND test_cogo_co)

    # co_st
    add_executable(test_co_st co_st_test.c)
    add_test(NAME test_co_st COMMAND test_co_st)
endif()
