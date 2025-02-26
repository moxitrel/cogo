# Copyright (c) 2025 Moxi Color
#
# Use of this source code is governed by a MIT-style license
# that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

include_guard(GLOBAL)

find_package(Doxygen OPTIONAL_COMPONENTS dot mscgen dia)

if(DOXYGEN_FOUND)
  block()
  include(FetchContent)

  #
  # doxygen-awesome-css
  #

  # FetchContent_Declare(doxygen-awesome-css
  # SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/github.com/jothepro/doxygen-awesome-css
  # )
  FetchContent_Declare(doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG v2.3.4
    GIT_SHALLOW Y
  )
  FetchContent_MakeAvailable(doxygen-awesome-css)

  set(DOXYGEN_GENERATE_TREEVIEW YES)
  set(DOXYGEN_DISABLE_INDEX NO)
  set(DOXYGEN_FULL_SIDEBAR NO)
  set(DOXYGEN_HTML_COLORSTYLE LIGHT)
  set(DOXYGEN_HTML_EXTRA_STYLESHEET
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome.css
    ${doxygen-awesome-css_SOURCE_DIR}/doxygen-awesome-sidebar-only.css
  )

  #
  # doxygen-awesome-css extensions
  #
  execute_process(
    COMMAND doxygen -w html header.html footer.html styleSheet.css
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )
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
  </head>
  ]])
  file(READ ${PROJECT_BINARY_DIR}/header.html _header_html)
  string(REPLACE "</head>" ${_install_extension} _header_html ${_header_html})
  file(WRITE ${PROJECT_BINARY_DIR}/header.html ${_header_html})

  set(DOXYGEN_HTML_EXTRA_STYLESHEET
    ${DOXYGEN_HTML_EXTRA_STYLESHEET}
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

  #
  # doxygen
  #
  set(DOXYGEN_EXCLUDE ${PROJECT_BINARY_DIR})
  set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
  set(DOXYGEN_QUIET YES)

  if(EXISTS ${PROJECT_SOURCE_DIR}/Doxyfile)
    doxygen_add_docs(_run_doxygen
      ALL
      CONFIG_FILE ${PROJECT_SOURCE_DIR}/Doxyfile
    )
  else()
    doxygen_add_docs(_run_doxygen
      ALL
    )
  endif()

  endblock()
endif()
