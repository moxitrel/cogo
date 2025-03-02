# Copyright (c) 2023 Moxi Color
#
# Use of this source code is governed by a MIT-style license
# that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

include_guard(GLOBAL)
include(FetchContent)
include(FindPackageMessage)
block()

string(TOLOWER "${CMAKE_BUILD_TYPE}" lowercase_CMAKE_BUILD_TYPE)

if(lowercase_CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
elseif(lowercase_CMAKE_BUILD_TYPE STREQUAL "debug")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

#
# FetchContent_Declare(Unity
# SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/github.com/ThrowTheSwitch/Unity
# )
#
FetchContent_Declare(unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.6.1
  GIT_SHALLOW Y
)

# Exclude from `cmake -install`
FetchContent_GetProperties(unity)

if(NOT unity_POPULATED)
  FetchContent_Populate(unity)
  add_subdirectory(${unity_SOURCE_DIR} ${unity_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

if(TARGET pch)
  target_precompile_headers(pch INTERFACE
    <unity.h>
  )
endif()

find_package_message(unity
  "Found unity-v2.6.1+${CMAKE_BUILD_TYPE}"
  "v2.6.1"
)

endblock()
