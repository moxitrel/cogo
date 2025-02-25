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

  set(_install_extension [[
    <script type="text/javascript" src="$relpath^doxygen-awesome-darkmode-toggle.js"></script>
    <script type="text/javascript" src="$relpath^doxygen-awesome-fragment-copy-button.js"></script>
    <script type="text/javascript" src="$relpath^doxygen-awesome-paragraph-link.js"></script>
    <script type="text/javascript" src="$relpath^doxygen-awesome-interactive-toc.js"></script>
    <script type="text/javascript">
      DoxygenAwesomeDarkModeToggle.init()
      DoxygenAwesomeFragmentCopyButton.init()
      DoxygenAwesomeParagraphLink.init()
      DoxygenAwesomeInteractiveToc.init()
    </script>
  ]])
  execute_process(
      COMMAND doxygen -w html header.html footer.html styleSheet.css
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
  file(READ ${PROJECT_BINARY_DIR}/header.html _header_html)
  string(CONCAT _install_extension ${_install_extension} "</head>")
  string(REPLACE "</head>" ${_install_extension} _header_html ${_header_html})
  file(WRITE ${PROJECT_BINARY_DIR}/header.html ${_header_html})

  set(_bak_DOXYGEN_HIDE_UNDOC_MEMBERS DOXYGEN_HIDE_UNDOC_MEMBERS)
  set(_bak_DOXYGEN_EXCLUDE DOXYGEN_EXCLUDE)
  set(_bak_DOXYGEN_QUIET DOXYGEN_QUIET)
  set(_bak_DOXYGEN_GENERATE_TREEVIEW DOXYGEN_GENERATE_TREEVIEW)
  set(_bak_DOXYGEN_DISABLE_INDEX DOXYGEN_DISABLE_INDEX)
  set(_bak_DOXYGEN_FULL_SIDEBAR DOXYGEN_FULL_SIDEBAR)
  set(_bak_DOXYGEN_HTML_COLORSTYLE DOXYGEN_HTML_COLORSTYLE)
  set(_bak_DOXYGEN_HTML_EXTRA_STYLESHEET DOXYGEN_HTML_EXTRA_STYLESHEET)
  set(_bak_DOXYGEN_HTML_HEADER DOXYGEN_HTML_HEADER)
  set(_bak_DOXYGEN_HTML_EXTRA_FILES DOXYGEN_HTML_EXTRA_FILES)
  set(_bak_DOXYGEN_HTML_COPY_CLIPBOARD DOXYGEN_HTML_COPY_CLIPBOARD)
  set(_bak_DOXYGEN_DOT_IMAGE_FORMAT DOXYGEN_DOT_IMAGE_FORMAT)
  set(_bak_DOXYGEN_DOT_TRANSPARENT DOXYGEN_DOT_TRANSPARENT)
  set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
  set(DOXYGEN_EXCLUDE ${PROJECT_BINARY_DIR})
  set(DOXYGEN_QUIET YES)
  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_DISABLE_INDEX NO)
  set(DOXYGEN_FULL_SIDEBAR NO)
  set(DOXYGEN_HTML_COLORSTYLE LIGHT)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-sidebar-only.css
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css
  )
  set(DOXYGEN_HTML_HEADER ${PROJECT_BINARY_DIR}/header.html)
  set(DOXYGEN_HTML_EXTRA_FILES
     ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js
     ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js
     ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-paragraph-link.js
     ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-interactive-toc.js
  )
  set(DOXYGEN_HTML_COPY_CLIPBOARD NO)
  set(DOXYGEN_DOT_IMAGE_FORMAT svg)
  set(DOXYGEN_DOT_TRANSPARENT YES)

  doxygen_add_docs(_doxygen
    ALL
  )

  set(DOXYGEN_HIDE_UNDOC_MEMBERS _bak_DOXYGEN_HIDE_UNDOC_MEMBERS)
  set(DOXYGEN_EXCLUDE _bak_DOXYGEN_EXCLUDE)
  set(DOXYGEN_QUIET _bak_DOXYGEN_QUIET)
  set(DOXYGEN_GENERATE_TREEVIEW _bak_DOXYGEN_GENERATE_TREEVIEW)
  set(DOXYGEN_DISABLE_INDEX _bak_DOXYGEN_DISABLE_INDEX)
  set(DOXYGEN_FULL_SIDEBAR _bak_DOXYGEN_FULL_SIDEBAR)
  set(DOXYGEN_HTML_COLORSTYLE _bak_DOXYGEN_HTML_COLORSTYLE)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET _bak_DOXYGEN_HTML_EXTRA_STYLESHEET)
  set(DOXYGEN_HTML_HEADER _bak_DOXYGEN_HTML_HEADER)
  set(DOXYGEN_HTML_EXTRA_FILES _bak_DOXYGEN_HTML_EXTRA_FILES)
  set(DOXYGEN_HTML_COPY_CLIPBOARD _bak_DOXYGEN_HTML_COPY_CLIPBOARD)
  set(DOXYGEN_DOT_IMAGE_FORMAT _bak_DOXYGEN_DOT_IMAGE_FORMAT)
  set(DOXYGEN_DOT_TRANSPARENT _bak_DOXYGEN_DOT_TRANSPARENT)
endif()
