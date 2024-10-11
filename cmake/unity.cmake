# MIT License
#
# Copyright (c) 2023 Moxi Color
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
include_guard(GLOBAL)
include(FetchContent)
include(FindPackageMessage)

string(TOLOWER "${CMAKE_BUILD_TYPE}" _lowercase_CMAKE_BUILD_TYPE)
set(_bak_CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})
string(TOLOWER "${CMAKE_BUILD_TYPE}" _lowercase_CMAKE_BUILD_TYPE)

if(_lowercase_CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE "Release")
elseif(_lowercase_CMAKE_BUILD_TYPE STREQUAL "debug")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

#
# FetchContent_Declare(Unity
# SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/github.com/ThrowTheSwitch/Unity
# )
#
FetchContent_Declare(unity # case-insensitive
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.6.0
  GIT_SHALLOW Y # git clone --depth 1
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
  "Found unity-v2.6.0+${CMAKE_BUILD_TYPE}"
  "v2.6.0"
)

set(CMAKE_BUILD_TYPE ${_bak_CMAKE_BUILD_TYPE})
