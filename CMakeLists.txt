cmake_minimum_required(VERSION 3.10)
project(cogo
  VERSION 3.0.0
  HOMEPAGE_URL "https://github.com/moxitrel/cogo"
  LANGUAGES C
)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(cmake/cogoCPack.cmake)
if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  include(CTest)
endif()

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME AND BUILD_TESTING)
  set(_default_COGO_TEST YES)
endif()

option(COGO_TEST "Build and run tests." ${_default_COGO_TEST})

file(GLOB_RECURSE cogo_PUBLIC_HEADER FOLLOW_SYMLINKS CONFIGURE_DEPENDS
  include/*.h
)

add_library(cogo
  src/cogo_await.c
  src/cogo_async.c
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
target_compile_definitions(cogo PUBLIC
  $<$<CONFIG:Debug>:COGO_DEBUG>
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

if(COGO_TEST)
  include(CheckCCompilerFlag)
  include(cmake/unity.cmake)
  include(cmake/compile_options.cmake)

  target_compile_options(cogo PRIVATE
    ${ZY_COMPILE_OPTIONS_DIAGNOSTIC}
    -Wno-padded
    -Wno-unsafe-buffer-usage
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

  add_compile_options(
    ${ZY_COMPILE_OPTIONS_DIAGNOSTIC}
    -Wno-padded
    -Wno-unsafe-buffer-usage
  )

  link_libraries(cogo unity)

  # pt
  add_executable(test_pt src/cogo_pt_test.c)
  add_test(NAME test_pt COMMAND test_pt)

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
    test_await
    test_async
  )
endif()
