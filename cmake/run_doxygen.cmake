# Copyright (c) 2025 Moxi Color
#
# Use of this source code is governed by a MIT-style license
# that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

include_guard(GLOBAL)

find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)

if(DOXYGEN_FOUND)
  include(FetchContent)
  include(FindPackageMessage)

  #
  # FetchContent_Declare(doxygen-awesome-css
  # SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/github.com/jothepro/doxygen-awesome-css
  # )
  #
  FetchContent_Declare(doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG v2.3.4
    GIT_SHALLOW Y
  )
  FetchContent_MakeAvailable(doxygen-awesome-css)

  set(_bak_DOXYGEN_HIDE_UNDOC_MEMBERS DOXYGEN_HIDE_UNDOC_MEMBERS)
  set(_bak_DOXYGEN_GENERATE_TREEVIEW DOXYGEN_GENERATE_TREEVIEW)
  set(_bak_DOXYGEN_DISABLE_INDEX DOXYGEN_DISABLE_INDEX)
  set(_bak_DOXYGEN_FULL_SIDEBAR DOXYGEN_FULL_SIDEBAR)
  set(_bak_DOXYGEN_HTML_EXTRA_STYLESHEET DOXYGEN_HTML_EXTRA_STYLESHEET)
  set(_bak_DOXYGEN_HTML_COLORSTYLE DOXYGEN_HTML_COLORSTYLE)

  set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_DISABLE_INDEX NO)
  set(DOXYGEN_FULL_SIDEBAR NO)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-sidebar-only.css
  )
  set(DOXYGEN_HTML_COLORSTYLE LIGHT)
  get_property(_cogo_DOXYGEN_INPUT
    TARGET cogo
    PROPERTY HEADER_SET
  )
  doxygen_add_docs(cogoDoxygen
    ${_cogo_DOXYGEN_INPUT}
    ALL
  )

  set(DOXYGEN_HIDE_UNDOC_MEMBERS _bak_DOXYGEN_HIDE_UNDOC_MEMBERS)
  set(DOXYGEN_GENERATE_TREEVIEW _bak_DOXYGEN_GENERATE_TREEVIEW)
  set(DOXYGEN_DISABLE_INDEX _bak_DOXYGEN_DISABLE_INDEX)
  set(DOXYGEN_FULL_SIDEBAR _bak_DOXYGEN_FULL_SIDEBAR)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET _bak_DOXYGEN_HTML_EXTRA_STYLESHEET)
  set(DOXYGEN_HTML_COLORSTYLE _bak_DOXYGEN_HTML_COLORSTYLE)
endif()
