# One CMakeLists.txt per target
#
# command (args ...)
#


# cmake_minimum_required(VERSION  <min>[...<max>])
# <min>, <max>: major.minor[.patch[.tweak]]
cmake_minimum_required(VERSION 3.12)


# STATIC: static library
# SHARED: shared library
# MODULE: dynamically loaded into and executable
# <not-set>: var BUILD_SHARED_LIBS | STATIC
add_library(<name>  [STATIC | SHARED | MODULE] [EXCLUDE_FROM_ALL])
# generate exe file named <name>
add_executable(<name>  [WIN32] [MACOSX_BUNDLE] [EXCLUDE_FROM_ALL])

# include files to search for compiler
# SYSTEM: as system include directories
# BEFORE: prepending instead of appending
target_include_directories(<target> [SYSTEM] [BEFORE]
        <INTERFACE|PUBLIC|PRIVATE> [items1...]
        [<INTERFACE|PUBLIC|PRIVATE> [items2...] ...])

# Collect all source files in <dir> and stores the list in <variable>
aux_source_directory(<dir> <variable>)

# sources files for given target.
# path relative to the directory where the add_library() call is made
# PRIVATE   : only add to target self                                 , e.g. sources, private headers
# INTERFACE : only add to anything links to target but not target self, e.g. headers
# PUBLIC    : both add to anything links to target and     target self, e.g. headers
target_sources(<target>
         <INTERFACE|PUBLIC|PRIVATE> [items1...]
        [<INTERFACE|PUBLIC|PRIVATE> [items2...]
         ...])

# libraries or flags to use when linking a given target and/or its dependents
# <target>: created by add_executable() or add_library()
# <item>  : A library target name        : created by add_library()
#         | A plain library name         : e.g. foo                becomes -lfoo or foo.lib
#         | A link flag                  : item name starting with -, but not -l or -framework
#         | A full path to a library file: e.g. /usr/lib/libfoo.so becomes -lfoo
#         | (debug | optimized | general) <item>
target_link_libraries(<target>
        <PRIVATE|PUBLIC|INTERFACE> <item>...
        [<PRIVATE|PUBLIC|INTERFACE> <item>...]...)


enable_testing()
# exit with 0 to pass and non-zero to fail.
# COMMAND          : test command-line | target (created by add_executable())
# CONFIGURATIONS   : Restrict execution of the test only to the named configurations
# WORKING_DIRECTORY: set teh working directory where to execute the test
add_test(NAME <name>
        COMMAND <command> [<arg>...]
        [CONFIGURATIONS <config>...]
        [WORKING_DIRECTORY <dir>])

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
find_library(<VAR>
          <name> | NAMES <name1> [name2 ...] [NAMES_PER_DIR]
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


set_property(<GLOBAL                      |
        DIRECTORY [dir]                   |
        TARGET    [target1 [target2 ...]] |
        SOURCE    [src1 [src2 ...]]       |
        INSTALL   [file1 [file2 ...]]     |
        TEST      [test1 [test2 ...]]     |
        CACHE     [entry1 [entry2 ...]]>
        [APPEND] [APPEND_STRING]
        PROPERTY <name> [value1 [value2 ...]])

get_property(<variable>
        <GLOBAL            |
        DIRECTORY [dir]    |
        TARGET    <target> |
        SOURCE    <source> |
        INSTALL   <file>   |
        TEST      <test>   |
        CACHE     <entry>  |
        VARIABLE>
        PROPERTY <name>
        [SET | DEFINED | BRIEF_DOCS | FULL_DOCS])