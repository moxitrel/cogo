cmake_minimum_required(VERSION 3.23)
project(cogo VERSION 1.0.0 LANGUAGES C)

include(CPack)
include(CTest)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(GNUInstallDirs)

option(COGO_NO_RESUME "CO_RESUME()" OFF)

cmake_dependent_option(COGO_USE_CASE "implement yield with switch case"
    FALSE "NOT COGO_USE_LABEL_VALUE" FALSE)
cmake_dependent_option(COGO_USE_LABEL_VALUE "implement yield with gcc lable-as-value extension"
    FALSE "NOT COGO_USE_CASE" FALSE)

add_library(cogo
    src/cogo_async.c
    src/cogo_await.c
)
target_sources(cogo INTERFACE FILE_SET HEADERS TYPE HEADERS
  BASE_DIRS
    include
  FILES
    include/cogo/_private/cogo_queue_template.h
    include/cogo/_private/cogo_yield_case.h
    include/cogo/_private/cogo_yield_label_value.h
    include/cogo/_private/macro_utils.h
    include/cogo/cogo_async.h
    include/cogo/cogo_await.h
    include/cogo/cogo_yield.h
)
target_include_directories(cogo PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(cogo PUBLIC c_std_99)
target_compile_definitions(cogo PUBLIC
    $<$<BOOL:${COGO_USE_CASE}>:COGO_USE_CASE>
    $<$<AND:$<NOT:$<BOOL:${COGO_USE_CASE}>>,$<BOOL:${COGO_USE_LABEL_VALUE}>>:COGO_USE_LABEL_VALUE>
    $<$<BOOL:${COGO_NO_RESUME}>:COGO_NO_RESUME>
    COGO_MP=1   # multithreaded scheduler
)
if(COGO_NO_WARNING)
  check_c_compiler_flag(-Wno-gnu-zero-variadic-macro-arguments _WNO_GNU_ZERO_VARIADIC_MACRO_ARGUMENT)
  check_c_compiler_flag(-Wno-gnu-label-as-value _WNO_GNU_LABEL_AS_VALUE)
  check_c_compiler_flag(-Wno-padded _WNO_PADDED)
  target_compile_options(cogo PUBLIC
      $<$<BOOL:${_WNO_GNU_LABEL_AS_VALUE}>:-Wno-gnu-label-as-value>
      $<$<BOOL:${_WNO_PADDED}>:-Wno-padded>
      $<$<BOOL:${_WNO_GNU_ZERO_VARIADIC_MACRO_ARGUMENT}>:-Wno-gnu-zero-variadic-macro-arguments>
  )
endif()

install(TARGETS cogo EXPORT cogo-targets
  ARCHIVE
  LIBRARY
  FILE_SET HEADERS
)

export(EXPORT cogo-targets
  FILE cogo-targets.cmake
)
install(EXPORT cogo-targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)

configure_package_config_file(cmake/cogo-config.cmake.in cogo-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)
write_basic_package_version_file(cogo-config-version.cmake
  COMPATIBILITY SameMajorVersion
)
install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cogo-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cogo-config-version.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)

if(BUILD_TESTING AND (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME))
    include(CheckCCompilerFlag)
    include(CMakePrintHelpers)
    include(cmake/unity.cmake)

    cmake_print_variables(COGO_USE_CASE COGO_USE_LABEL_VALUE CMAKE_C_COMPILER)

    check_c_compiler_flag(-Wc++-compat _W_CPP_COMPAT)
    check_c_compiler_flag(-Weverything _W_EVERYTHING)
    check_c_compiler_flag(-Wall _W_ALL)
    check_c_compiler_flag(-Wextra _W_EXTRA)
    check_c_compiler_flag(-Wconversion _W_CONVERSION)
    check_c_compiler_flag(-fanalyzer _F_ANALYZER)
    check_c_compiler_flag(-Werror _W_ERROR)

    check_c_compiler_flag(-Wno-padded _WNO_PADDED)
    check_c_compiler_flag(-Wno-declaration-after-statement _WNO_DECLARATION_AFTER_STATEMENT)
    check_c_compiler_flag(-Wno-gnu-zero-variadic-macro-arguments _WNO_GNU_ZERO_VARIADIC_MACRO_ARGUMENT)
    check_c_compiler_flag(-Wno-gnu-label-as-value _WNO_GNU_LABEL_AS_VALUE)
    check_c_compiler_flag(-Wno-gnu-empty-initializer _WNO_GNU_EMPTY_INITIALIZER)
    check_c_compiler_flag(-Wno-missing-prototypes _WNO_MISSING_PROTOTYPES)
    check_c_compiler_flag(-Wno-unreachable-code _WNO_UNREACHABLE_CODE)
    check_c_compiler_flag(-Wno-missing-declarations _WNO_MISSING_DECLARATIONS)
    check_c_compiler_flag(-Wno-missing-field-initializers _WNO_MISSING_FIELD_INITIALIZERS)
    check_c_compiler_flag(-Wno-suggest-attribute _WNO_SUGGEST_ATTRIBUTE)
    check_c_compiler_flag(-Wno-unused-function _WNO_UNUSED_FUNCTION)

    target_compile_options(cogo PRIVATE
        $<$<BOOL:${_W_CPP_COMPAT}>:-Wc++-compat>
        $<$<BOOL:${_W_EVERYTHING}>:-Weverything>
        $<$<BOOL:${_W_ALL}>:-Wall>
        $<$<BOOL:${_W_EXTRA}>:-Wextra>
        $<$<BOOL:${_W_CONVERSION}>:-Wconversion>
        $<$<BOOL:${_F_ANALYZER}>:-fanalyzer>
        $<$<BOOL:${_W_ERROR}>:-Werror>

        $<$<BOOL:${_WNO_DECLARATION_AFTER_STATEMENT}>:-Wno-declaration-after-statement>
        $<$<BOOL:${_WNO_PADDED}>:-Wno-padded>
    )

    add_compile_options(
        $<$<BOOL:${_WNO_GNU_ZERO_VARIADIC_MACRO_ARGUMENT}>:-Wno-gnu-zero-variadic-macro-arguments>
        $<$<BOOL:${_WNO_GNU_LABEL_AS_VALUE}>:-Wno-gnu-label-as-value>
        $<$<BOOL:${_WNO_PADDED}>:-Wno-padded>

        $<$<BOOL:${_W_ERROR}>:-Werror>
        $<$<BOOL:${_W_EVERYTHING}>:-Weverything>
        $<$<BOOL:${_WNO_GNU_EMPTY_INITIALIZER}>:-Wno-gnu-empty-initializer>
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

    link_libraries(cogo unity)

    # yield
    add_executable(test_yield src/cogo_yield_test.c)
    add_test(NAME test_yield COMMAND test_yield)

    # await
    add_executable(test_await src/cogo_await_test.c)
    add_test(NAME test_await COMMAND test_await)

    # async
    add_executable(test_async src/cogo_async_test.c)
    add_test(NAME test_async COMMAND test_async)
endif()
