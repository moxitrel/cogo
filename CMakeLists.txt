#
# command (args ...)
#

# cmake_minimum_required(
#   VERSION major.minor[.patch[.tweak]]
#   [FATAL_ERROR])
cmake_minimum_required(VERSION)

# Top-level CMakeLists.txt must contain project() command
#
# Set PROJECT_NAME                  : <PROJECT-NAME>
#     PROJECT_VERSION               : <major>[.<minor>[.<patch>[.<tweak>]]]]
#    <PROJECT-NAME>_VERSION         : <major>[.<minor>[.<patch>[.<tweak>]]]]
#     PROJECT_VERSION_MAJOR         : <major>
#    <PROJECT-NAME>_VERSION_MAJOR   : <major>
#     PROJECT_VERSION_MINOR         : <minor>   |   ""
#    <PROJECT-NAME>_VERSION_MINOR   : <minor>   |   ""
#     PROJECT_VERSION_PATCH         : <patch>   |   ""
#    <PROJECT-NAME>_VERSION_PATCH   : <patch>   |   ""
#     PROJECT_VERSION_TWEAK         : <tweak>   |   ""
#    <PROJECT-NAME>_VERSION_TWEAK   : <tweak>   |   ""
#     PROJECT_DESCRIPTION           : <project-description-string>
#
# LANGUAGES: C | CXX | ASM, default to C and C++
#   | NONE: no language specific
#   C++ : C support will be loaded too
project(<PROJECT-NAME>
        VERSION 0.0.0           #<major>[.<minor>[.<patch>[.<tweak>]]]]
#        DESCRIPTION <project-description-string>
        LANGUAGES C CXX ASM
        )

# Add a subdirectory to the build
add_subdirectory(source_dir [binary_dir]
                 [EXCLUDE_FROM_ALL]         #not in top level Makefile, e.g. examples
                 )

#
# set(VAR v1 v2 ...): VAR -> v1 v2 ...
#  ${VAR}           : expand to list        , e.g.  v1 v2 ...
# "${VAR}"          : expand to single value, e.g. "v1 v2 ..."
# $ENV{VAR}         : value of system environment variable
# [HKEY_CURRENT_USER\\Software\\path1\\path2;key] : value of windows registry
#
set(CMAKE_C_STANDARD 11)

list(REMOVE_ITEM <list> <value> [<value> ...])
list(REMOVE_AT   <list> <index> [<index> ...])
separate_arguments(<var> <NATIVE|UNIX|WINDOWS>_COMMAND "<args>")

# true : 1 | ON  | YES | TRUE  | Y | a non-zero number
# false: 0 | OFF | NO  | FALSE | N | IGNORE | NOTFOUND | "" | ends in the suffix -NOTFOUND
# exp  : x
#      | NOT x
#      | x AND x
#      | x OR x
#      | COMMAND cmdname    #true if cmdname is a command can be invoked
#      | DEFINED x          #true if x is set
#      | EXISTS file        #true if file exists
#      | EXISTS dir         #true if dir exists
#      | IS_DIRECTORY name  #true if name is a dir
#      | IS_ABSOLUTE  name  #true if name is absolute path
#      | name1 IS_NEWER_THAN name2  #true if file name1 is more recent modified
#      | x   MATCHES regex  #true if x's value match regex
#      | str MATCHES regex  #true if string str match regex
#      | n   (   LESS |    EQUAL |    GREATER) n
#      | str (STRLESS | STREQUAL | STRGREATER) str
#      | ver (VERSION_LESS | VERSION_EQUAL | VERSION_GREATER) ver
if(exp)
  COMMAND1(ARGS ...)
  #...
elseif(exp2)
  COMMAND1(ARGS ...)
  #...
else(exp)
  COMMAND1(ARGS ...)
  #...
endif(exp)

#
foreach(loop_var arg1 arg2 ...)
  COMMAND1(ARGS ...)
  #...
endforeach(loop_var)

# <VAR>: store the result
#        If nothing found, the result will be <VAR>-NOTFOUND
# NAMES: possible names for the library
# HINTS, PATHS: dirs to search in addition to the default locations
#               ENV: paths read from system environment variable var
find_library (
          <VAR>
          name | NAMES name1 [name2 ...] [NAMES_PER_DIR]
          [HINTS path1 [path2 ... ENV var]]
          [PATHS path1 [path2 ... ENV var]]
          [PATH_SUFFIXES suffix1 [suffix2 ...]]
          [DOC "cache documentation string"]
          [NO_DEFAULT_PATH]
          [NO_CMAKE_PATH]
          [NO_CMAKE_ENVIRONMENT_PATH]
          [NO_SYSTEM_ENVIRONMENT_PATH]
          [NO_CMAKE_SYSTEM_PATH]
          [CMAKE_FIND_ROOT_PATH_BOTH |
           ONLY_CMAKE_FIND_ROOT_PATH |
           NO_CMAKE_FIND_ROOT_PATH]
         )

# <not-set>: var BUILD_SHARED_LIBS | STATIC
# STATIC: static library
# SHARED: shared library
# MODULE: dynamically loaded into and executable
add_library(<name> [STATIC | SHARED | MODULE]
            [EXCLUDE_FROM_ALL]
            [source1] [source2 ...]
            )

# gen exe file named <name>
# source files can be added later using target_sources()
add_executable(<name> [WIN32] [MACOSX_BUNDLE]
               [EXCLUDE_FROM_ALL]
               [source1] [source2 ...]
               )

# <target>: created by add_executable() or add_library()
# <item>  : A library target name
#         | A full path to a library file
#         | A plain library name
#         | A link flag
#         | (debug | optimized | general) <item>
target_link_libraries(<target> ... <item>... ...)

set_target_properties(target1 target2 ...
                      PROPERTIES prop1 value1
                      prop2 value2 ...)
# VAR : target.property
get_target_property(VAR target property)

set_source_files_properties([file1 [file2 [...]]]
                            PROPERTIES prop1 value1
                            [prop2 value2 [...]]
                            )
# VAR : file.property
get_source_file_property(VAR file property)