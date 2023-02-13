cmake_minimum_required(VERSION 3.13)
project(cogo)

include(CTest)
include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

check_c_compiler_flag(-Wno-padded _WNO_PADDED)

add_library(cogo
    cogo_co.c
    co_st.c
)
target_include_directories(cogo PUBLIC include)
target_compile_features(cogo PUBLIC c_std_99)
target_compile_options(cogo PUBLIC
    $<$<BOOL:${_WNO_PADDED}>:-Wno-padded>
    ${PROJECT_COMPILE_OPTIONS_STATIC_ANALYSIS}
)

if (BUILD_TESTING AND (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME))
    include(cmake/unity.cmake)

    # set(CMAKE_C_COMPILER    clang++)

    check_c_compiler_flag(-Wno-extra-semi-stmt WNO_EXTRA_SEMI_STMT)
    check_c_compiler_flag(-Wno-missing-prototypes WNO_MISSING_PROTOTYPES)
    check_c_compiler_flag(-Wno-missing-declarations WNO_MISSING_DECLARATIONS)
    check_c_compiler_flag(-Wno-unreachable-code WNO_UNREACHABLE_CODE)
    check_c_compiler_flag(-Wno-missing-field-initializers WNO_MISSING_FIELD_INITIALIZERS)
    check_c_compiler_flag(-Wno-old-style-cast WNO_OLD_STYLE_CAST)
    check_c_compiler_flag(-Wno-suggest-attribute WNO_SUGGEST_ATTRIBUTE)
    add_compile_options(
        ${PROJECT_COMPILE_OPTIONS_STATIC_ANALYSIS}

        $<$<BOOL:${WNO_MISSING_PROTOTYPES}>:-Wno-missing-prototypes>
        $<$<BOOL:${WNO_UNREACHABLE_CODE}>:-Wno-unreachable-code>
        $<$<BOOL:${WNO_MISSING_DECLARATIONS}>:-Wno-missing-declarations>
        $<$<BOOL:${WNO_MISSING_FIELD_INITIALIZERS}>:-Wno-missing-field-initializers>
        $<$<BOOL:${WNO_SUGGEST_ATTRIBUTE}>:-Wno-suggest-attribute=const>
        $<$<BOOL:${WNO_SUGGEST_ATTRIBUTE}>:-Wno-suggest-attribute=pure>
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
