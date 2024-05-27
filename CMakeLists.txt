cmake_minimum_required(VERSION 3.23)
project(cogo
  VERSION 1.0.0
  HOMEPAGE_URL "https://github.com/moxitrel/cogo"
  LANGUAGES C
)
include(CheckCCompilerFlag)
include(CMakePackageConfigHelpers)
include(CMakeDependentOption)
include(CTest)
include(GNUInstallDirs)
include(cmake/cogoCPack.cmake)

option(COGO_BUILD_TESTING "Build tests."
  OFF
)
cmake_dependent_option(COGO_USE_SWITCH "Implement CO_YIELD with switch case."
  OFF "NOT COGO_USE_LABELS_AS_VALUES" OFF
)
cmake_dependent_option(COGO_USE_LABELS_AS_VALUES "Implement CO_YIELD with GCC extension of label value."
  OFF "NOT COGO_USE_SWITCH" OFF
)

add_library(cogo
  src/cogo_async.c
  src/cogo_await.c
)
target_sources(cogo INTERFACE
  FILE_SET HEADERS
  TYPE HEADERS
  BASE_DIRS include
  FILES
  include/cogo/private/cogo_queue_template.h
  include/cogo/private/cogo_yield_labels_as_values.h
  include/cogo/private/cogo_yield_switch.h
  include/cogo/private/macro_utils.h
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
  $<$<BOOL:${COGO_USE_LABELS_AS_VALUES}>:COGO_USE_LABELS_AS_VALUES>
  $<$<BOOL:${COGO_USE_SWITCH}>:COGO_USE_SWITCH>
  COGO_MP=1 # multithreaded scheduler
)
check_c_compiler_flag(-Wc++-compat _Wcpp_compat)
check_c_compiler_flag(-Wpedantic _Wpedantic)
check_c_compiler_flag(-Wall _Wall)
check_c_compiler_flag(-Wextra _Wextra)
check_c_compiler_flag(-Wconversion _Wconversion)
check_c_compiler_flag(-Wsign-conversion _Wsign_conversion)
check_c_compiler_flag(-Warith-conversion _Warith_conversion)
check_c_compiler_flag(-Wcast-qual _Wcast_qual)
check_c_compiler_flag(-Wcast-align=strict _Wcast_align_strict)
check_c_compiler_flag(-fanalyzer _fanalyzer)
target_compile_options(cogo PRIVATE
  $<$<BOOL:${_Wcpp_compat}>:-Wc++-compat>
  $<$<BOOL:${_Wpedantic}>:-Wpedantic>
  $<$<BOOL:${_Wall}>:-Wall>
  $<$<BOOL:${_Wextra}>:-Wextra>
  $<$<BOOL:${_Wconversion}>:-Wconversion>
  $<$<BOOL:${_Wsign_conversion}>:-Wsign-conversion>
  $<$<BOOL:${_Warith_conversion}>:-Warith-conversion>
  $<$<BOOL:${_Wcast_qual}>:-Wcast-qual>
  $<$<BOOL:${_Wcast_align_strict}>:-Wcast-align=strict>
  $<$<BOOL:${_fanalyzer}>:-fanalyzer>
)
install(TARGETS cogo EXPORT cogoTargets
  ARCHIVE
  LIBRARY
  FILE_SET HEADERS
)
add_library(cogo::cogo ALIAS cogo)

export(EXPORT cogoTargets)

install(EXPORT cogoTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)
configure_package_config_file(cmake/cogoConfig.cmake.in cogoConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)
write_basic_package_version_file(cogoConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)
install(
  FILES
  ${CMAKE_CURRENT_BINARY_DIR}/cogoConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cogoConfigVersion.cmake
  DESTINATION
  ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)

configure_file(cmake/cogo.pc.in cogo.pc @ONLY)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/cogo.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

if(COGO_BUILD_TESTING OR((PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME) AND BUILD_TESTING))
  include(cmake/unity.cmake)

  message(STATUS "COGO_USE_SWITCH          : ${COGO_USE_SWITCH}")
  message(STATUS "COGO_USE_LABELS_AS_VALUES: ${COGO_USE_LABELS_AS_VALUES}")
  message(STATUS "CMAKE_C_COMPILER         : ${CMAKE_C_COMPILER}")

  check_c_compiler_flag(-Weverything _Weverything)
  check_c_compiler_flag(-Werror _Werror)

  check_c_compiler_flag(-Wno-padded _Wno_padded)
  check_c_compiler_flag(-Wno-declaration-after-statement _Wno_declaration_after_statement)
  check_c_compiler_flag(-Wno-gnu-zero-variadic-macro-arguments _Wno_gnu_zero_variadic_macro_arguments)
  check_c_compiler_flag(-Wno-gnu-label-as-value _Wno_gnu_label_as_value)

  check_c_compiler_flag(-Wno-missing-prototypes _Wno_missing_prototypes)
  check_c_compiler_flag(-Wno-unreachable-code _Wno_unreachable_code)
  check_c_compiler_flag(-Wno-missing-field-initializers _Wno_missing_field_initializers)
  check_c_compiler_flag(-Wno-unsafe-buffer-usage _Wno_unsafe_buffer_usage)
  check_c_compiler_flag(-Wno-gnu-empty-initializer _Wno_gnu_empty_initializer)

  target_compile_options(cogo PRIVATE
    $<$<BOOL:${_Werror}>:-Werror>
    $<$<BOOL:${_Weverything}>:-Weverything>

    $<$<BOOL:${_Wno_padded}>:-Wno-padded>
    $<$<BOOL:${_Wno_declaration_after_statement}>:-Wno-declaration-after-statement>
  )

  add_compile_options(
    $<$<BOOL:${_Werror}>:-Werror>
    $<$<BOOL:${_Weverything}>:-Weverything>

    # $<$<BOOL:${_Wpedantic}>:-Wpedantic>
    $<$<BOOL:${_Wcpp_compat}>:-Wc++-compat>
    $<$<BOOL:${_Wall}>:-Wall>
    $<$<BOOL:${_Wextra}>:-Wextra>
    $<$<BOOL:${_Wconversion}>:-Wconversion>
    $<$<BOOL:${_Wsign_conversion}>:-Wsign-conversion>
    $<$<BOOL:${_Warith_conversion}>:-Warith-conversion>
    $<$<BOOL:${_Wcast_qual}>:-Wcast-qual>
    $<$<BOOL:${_Wcast_align_strict}>:-Wcast-align=strict>
    $<$<BOOL:${_fanalyzer}>:-fanalyzer>

    $<$<BOOL:${_Wno_padded}>:-Wno-padded>
    $<$<BOOL:${_Wno_declaration_after_statement}>:-Wno-declaration-after-statement>
    $<$<BOOL:${_Wno_gnu_zero_variadic_macro_arguments}>:-Wno-gnu-zero-variadic-macro-arguments>
    $<$<BOOL:${_Wno_gnu_label_as_value}>:-Wno-gnu-label-as-value>

    $<$<BOOL:${_Wno_missing_prototypes}>:-Wno-missing-prototypes>
    $<$<BOOL:${_Wno_missing_field_initializers}>:-Wno-missing-field-initializers>
    $<$<BOOL:${_Wno_unreachable_code}>:-Wno-unreachable-code>
    $<$<BOOL:${_Wno_unsafe_buffer_usage}>:-Wno-unsafe-buffer-usage>
    $<$<BOOL:${_Wno_gnu_empty_initializer}>:-Wno-gnu-empty-initializer>
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
