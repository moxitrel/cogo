cmake_minimum_required(VERSION 3.13)
project(cogo)

include(CTest)
include(CheckCCompilerFlag)
include(CMakeDependentOption)

cmake_dependent_option(COGO_USE_CASE "implement yield with switch case"
    FALSE "NOT COGO_USE_LABEL_VALUE" FALSE)
cmake_dependent_option(COGO_USE_LABEL_VALUE "implement yield with gcc lable-as-value extension"
    FALSE "NOT COGO_USE_CASE" FALSE)

add_library(cogo
    src/cogo_await.c
    src/cogo_co.c
    src/cogo_st.c
)
target_include_directories(cogo PUBLIC include)
target_compile_features(cogo PUBLIC c_std_99)
target_compile_definitions(cogo PUBLIC
    $<$<BOOL:${COGO_USE_CASE}>:COGO_USE_CASE>
    $<$<BOOL:${COGO_USE_LABEL_VALUE}>:COGO_USE_LABEL_VALUE>
)
check_c_compiler_flag(-Wc++-compat _W_CPP_COMPAT)
check_c_compiler_flag(-Wall _W_ALL)
check_c_compiler_flag(-Wextra _W_EXTRA)
check_c_compiler_flag(-Wconversion _W_CONVERSION)
check_c_compiler_flag(-fanalyzer _F_ANALYZER)
target_compile_options(cogo PRIVATE
    $<$<BOOL:${_W_CPP_COMPAT}>:-Wc++-compat>
    $<$<BOOL:${_W_ALL}>:-Wall>
    $<$<BOOL:${_W_EXTRA}>:-Wextra>
    $<$<BOOL:${_W_CONVERSION}>:-Wconversion>
    $<$<BOOL:${_F_ANALYZER}>:-fanalyzer>
)

if(BUILD_TESTING AND(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME))
    include(CMakePrintHelpers)
    include(cmake/unity.cmake)

    cmake_print_variables(COGO_USE_CASE COGO_USE_LABEL_VALUE)

    check_c_compiler_flag(-Wno-gnu-zero-variadic-macro-arguments _WNO_GNU_ZERO_VARIADIC_MACRO_ARGUMENT)
    check_c_compiler_flag(-Wno-gnu-label-as-value _WNO_GNU_LABEL_AS_VALUE)
    check_c_compiler_flag(-Wno-padded _WNO_PADDED)

    check_c_compiler_flag(-Werror _W_ERROR)
    check_c_compiler_flag(-Weverything _W_EVERYTHING)
    check_c_compiler_flag(-Wno-missing-prototypes _WNO_MISSING_PROTOTYPES)
    check_c_compiler_flag(-Wno-unreachable-code _WNO_UNREACHABLE_CODE)
    check_c_compiler_flag(-Wno-missing-declarations _WNO_MISSING_DECLARATIONS)
    check_c_compiler_flag(-Wno-missing-field-initializers _WNO_MISSING_FIELD_INITIALIZERS)
    check_c_compiler_flag(-Wno-suggest-attribute _WNO_SUGGEST_ATTRIBUTE)
    check_c_compiler_flag(-Wno-unused-function _WNO_UNUSED_FUNCTION)
    add_compile_options(
        $<$<BOOL:${_WNO_GNU_ZERO_VARIADIC_MACRO_ARGUMENT}>:-Wno-gnu-zero-variadic-macro-arguments>
        $<$<BOOL:${_WNO_GNU_LABEL_AS_VALUE}>:-Wno-gnu-label-as-value>
        $<$<BOOL:${_WNO_PADDED}>:-Wno-padded>

        $<$<BOOL:${_W_ERROR}>:-Werror>
        $<$<BOOL:${_W_EVERYTHING}>:-Weverything>
        $<$<BOOL:${_W_ALL}>:-Wall>
        $<$<BOOL:${_W_EXTRA}>:-Wextra>
        $<$<BOOL:${_W_CONVERSION}>:-Wconversion>
        $<$<BOOL:${_F_ANALYZER}>:-fanalyzer>
        $<$<BOOL:${_WNO_MISSING_PROTOTYPES}>:-Wno-missing-prototypes>
        $<$<BOOL:${_WNO_UNREACHABLE_CODE}>:-Wno-unreachable-code>
        $<$<BOOL:${_WNO_MISSING_DECLARATIONS}>:-Wno-missing-declarations>
        $<$<BOOL:${_WNO_MISSING_FIELD_INITIALIZERS}>:-Wno-missing-field-initializers>
        $<$<BOOL:${_WNO_SUGGEST_ATTRIBUTE}>:-Wno-suggest-attribute=const>
        $<$<BOOL:${_WNO_SUGGEST_ATTRIBUTE}>:-Wno-suggest-attribute=pure>
        $<$<BOOL:${_WNO_UNUSED_FUNCTION}>:-Wno-unused-function>
    )

    link_libraries(cogo)

    # yield
    add_executable(test_yield src/cogo_yield_test.c)
    add_test(NAME test_yield COMMAND test_yield)

    # await
    add_executable(test_await src/cogo_await_test.c)
    add_test(NAME test_await COMMAND test_await)

    # channel
    add_executable(test_chan src/cogo_co_test.c)
    add_test(NAME test_chan COMMAND test_chan)
endif()
