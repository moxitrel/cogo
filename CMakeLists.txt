cmake_minimum_required(VERSION 3.10)
project(cogo
  VERSION 2.0.0
  HOMEPAGE_URL "https://github.com/moxitrel/cogo"
  LANGUAGES C
)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CTest)
include(cmake/cogoCPack.cmake)

option(COGO_TEST "Build and run tests."
  YES
)
option(COGO_USE_COMPUTED_GOTO "Use GCC extension (labels as values) to implement the coroutine."
  NO
)

file(GLOB_RECURSE cogo_PUBLIC_HEADER FOLLOW_SYMLINKS CONFIGURE_DEPENDS
  include/*.h
)

add_library(cogo
  src/cogo_async.c
  src/cogo_await.c
  src/cogo_yield.c
)
target_sources(cogo INTERFACE
  FILE_SET HEADERS
  BASE_DIRS include
  FILES ${cogo_PUBLIC_HEADER}
)
target_include_directories(cogo PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(cogo PUBLIC c_std_99)
target_compile_definitions(cogo PUBLIC
  $<$<BOOL:${COGO_USE_COMPUTED_GOTO}>:COGO_USE_COMPUTED_GOTO>
  $<$<CONFIG:Debug>:COGO_DEBUG>
  COGO_MP=1 # multithreaded scheduler
)
install(TARGETS cogo EXPORT cogoTargets
  ARCHIVE
  FILE_SET HEADERS
)

export(EXPORT cogoTargets)
configure_package_config_file(cmake/cogoConfig.cmake.in cogoConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)
write_basic_package_version_file(cogoConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)
export(PACKAGE cogo)
install(EXPORT cogoTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)
install(
  FILES
    ${PROJECT_BINARY_DIR}/cogoConfig.cmake
    ${PROJECT_BINARY_DIR}/cogoConfigVersion.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/cogo
)
configure_file(cmake/cogo.pc.in cogo.pc @ONLY)
install(
  FILES ${PROJECT_BINARY_DIR}/cogo.pc
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

include(cmake/run_doxygen.cmake)

if(COGO_TEST OR(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME AND BUILD_TESTING))
  include(CheckCCompilerFlag)
  include(cmake/unity.cmake)

  message(STATUS "COGO_USE_COMPUTED_GOTO : ${COGO_USE_COMPUTED_GOTO}")
  message(STATUS "CMAKE_C_COMPILER       : ${CMAKE_C_COMPILER}")

  check_c_compiler_flag(-Wc++-compat _Wcpp_compat)
  check_c_compiler_flag(-Wpedantic _Wpedantic)
  check_c_compiler_flag(-Wno-padded _Wno_padded)
  check_c_compiler_flag(-Wno-unsafe-buffer-usage _Wno_unsafe_buffer_usage)
  check_c_compiler_flag(-Wno-gnu-label-as-value _Wno_gnu_label_as_value)
  check_c_compiler_flag(-Werror _Werror)
  check_c_compiler_flag(-Weverything _Weverything)
  check_c_compiler_flag(-Wall _Wall)
  check_c_compiler_flag(-Wextra _Wextra)
  check_c_compiler_flag(-Wconversion _Wconversion)
  check_c_compiler_flag(-Wsign-conversion _Wsign_conversion)
  check_c_compiler_flag(-Warith-conversion _Warith_conversion)
  check_c_compiler_flag(-Wcast-qual _Wcast_qual)
  check_c_compiler_flag(-Wcast-align=strict _Wcast_align_strict)
  check_c_compiler_flag(-fanalyzer _fanalyzer)
  set(_compile_options
    $<$<BOOL:${_Wcpp_compat}>:-Wc++-compat>
    $<$<BOOL:${_Wno_padded}>:-Wno-padded>
    $<$<BOOL:${_Wno_unsafe_buffer_usage}>:-Wno-unsafe-buffer-usage>
    $<$<AND:$<NOT:$<BOOL:${COGO_USE_COMPUTED_GOTO}>>,$<BOOL:${_Wpedantic}>>:-Wpedantic>
    $<$<AND:$<BOOL:${COGO_USE_COMPUTED_GOTO}>,$<BOOL:${_Wno_gnu_label_as_value}>>:-Wno-gnu-label-as-value>
    $<$<BOOL:${_Werror}>:-Werror>
    $<$<BOOL:${_Weverything}>:-Weverything>
    $<$<BOOL:${_Wall}>:-Wall>
    $<$<BOOL:${_Wextra}>:-Wextra>
    $<$<BOOL:${_Wconversion}>:-Wconversion>
    $<$<BOOL:${_Wsign_conversion}>:-Wsign-conversion>
    $<$<BOOL:${_Warith_conversion}>:-Warith-conversion>
    $<$<BOOL:${_Wcast_qual}>:-Wcast-qual>
    $<$<BOOL:${_Wcast_align_strict}>:-Wcast-align=strict>
    $<$<BOOL:${_fanalyzer}>:-fanalyzer>
  )
  target_compile_options(cogo PRIVATE
    ${_compile_options}
  )
  target_compile_features(cogo PRIVATE
    c_std_90
  )
  set_target_properties(cogo PROPERTIES
    C_EXTENSIONS N
  )

  check_c_compiler_flag(-Wno-pre-c11-compat _Wno_pre_c11_compat)
  target_compile_options(unity PUBLIC
    $<$<BOOL:${_Wno_pre_c11_compat}>:-Wno-pre-c11-compat>
  )

  check_c_compiler_flag(-Wno-declaration-after-statement _Wno_declaration_after_statement)
  check_c_compiler_flag(-Wno-missing-prototypes _Wno_missing_prototypes)
  check_c_compiler_flag(-Wno-gnu-zero-variadic-macro-arguments _Wno_gnu_zero_variadic_macro_arguments)
  check_c_compiler_flag(-Wno-unreachable-code _Wno_unreachable_code)
  check_c_compiler_flag(-Wno-missing-field-initializers _Wno_missing_field_initializers)
  check_c_compiler_flag(-Wno-gnu-empty-initializer _Wno_gnu_empty_initializer)
  add_compile_options(
    ${_compile_options}
    $<$<BOOL:${_Wno_declaration_after_statement}>:-Wno-declaration-after-statement>
    $<$<BOOL:${_Wno_missing_prototypes}>:-Wno-missing-prototypes>

    # $<$<BOOL:${_Wno_gnu_zero_variadic_macro_arguments}>:-Wno-gnu-zero-variadic-macro-arguments>
    # $<$<BOOL:${_Wno_missing_field_initializers}>:-Wno-missing-field-initializers>
    # $<$<BOOL:${_Wno_unreachable_code}>:-Wno-unreachable-code>
    # $<$<BOOL:${_Wno_gnu_empty_initializer}>:-Wno-gnu-empty-initializer>
  )

  link_libraries(cogo unity)

  # pt
  add_executable(test_pt src/cogo_pt_test.c)
  add_test(NAME test_pt COMMAND test_pt)

  # yield
  add_executable(test_yield src/cogo_yield_test.c)
  add_test(NAME test_yield COMMAND test_yield)

  # await
  add_executable(test_await src/cogo_await_test.c)
  add_test(NAME test_await COMMAND test_await)

  # async
  add_executable(test_async src/cogo_async_test.c)
  add_test(NAME test_async COMMAND test_async)

  add_custom_target(_RunTests ALL VERBATIM
    COMMAND ctest --output-on-failure --schedule-random -T MemCheck -j2
  )
  add_dependencies(_RunTests
    test_pt
    test_yield
    test_await
    test_async
  )
endif()
