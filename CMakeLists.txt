cmake_minimum_required(VERSION 3.13)
project(cogo)

include(CTest)
include(CheckCCompilerFlag)

add_library(cogo
    src/cogo_await.c
    src/cogo_co.c
    src/cogo_st.c
)
target_include_directories(cogo PUBLIC include)
target_compile_features(cogo PUBLIC c_std_99)

if(BUILD_TESTING AND(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME))
    include(cmake/unity.cmake)

    # check_c_compiler_flag(-Wno-padded _WNO_PADDED)
    # check_c_compiler_flag(-Wno-pedantic _WNO_PEDANTIC)
    check_c_compiler_flag(-Wno-missing-prototypes WNO_MISSING_PROTOTYPES)
    check_c_compiler_flag(-Wno-unreachable-code WNO_UNREACHABLE_CODE)
    check_c_compiler_flag(-Wno-missing-declarations WNO_MISSING_DECLARATIONS)
    check_c_compiler_flag(-Wno-missing-field-initializers WNO_MISSING_FIELD_INITIALIZERS)
    check_c_compiler_flag(-Wno-suggest-attribute WNO_SUGGEST_ATTRIBUTE)
    add_compile_options(

        # $<$<BOOL:${_WNO_PADDED}>:-Wno-padded>
        # $<$<BOOL:${_WNO_PEDANTIC}>:-Wno-pedantic>
        $<$<BOOL:${WNO_MISSING_PROTOTYPES}>:-Wno-missing-prototypes>
        $<$<BOOL:${WNO_UNREACHABLE_CODE}>:-Wno-unreachable-code>
        $<$<BOOL:${WNO_MISSING_DECLARATIONS}>:-Wno-missing-declarations>
        $<$<BOOL:${WNO_MISSING_FIELD_INITIALIZERS}>:-Wno-missing-field-initializers>
        $<$<BOOL:${WNO_SUGGEST_ATTRIBUTE}>:-Wno-suggest-attribute=const>
        $<$<BOOL:${WNO_SUGGEST_ATTRIBUTE}>:-Wno-suggest-attribute=pure>
    )

    link_libraries(cogo)

    # yield: case
    add_executable(test_yield_case src/cogo_yield_test.c)
    target_compile_definitions(test_yield_case PRIVATE COGO_YIELD_CASE)
    add_test(NAME test_yield_case COMMAND test_yield_case)

    # yield: label value
    add_executable(test_yield_label_value src/cogo_yield_test.c)
    target_compile_definitions(test_yield_label_value PRIVATE COGO_YIELD_LABEL_VALUE)
    add_test(NAME test_yield_label_value COMMAND test_yield_label_value)

    # await
    add_executable(test_await src/cogo_await_test.c)
    add_test(NAME test_await COMMAND test_await)

    # channel
    add_executable(test_chan src/cogo_co_test.c)
    add_test(NAME test_chan COMMAND test_chan)
endif()
