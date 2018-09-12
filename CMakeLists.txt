# One CMakeLists.txt per target
#
# command (args ...)
#


# cmake_minimum_required(VERSION  <min>[...<max>])
# <min>, <max>: major.minor[.patch[.tweak]]
cmake_minimum_required(VERSION 3.11)

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
        VERSION 0.0.0           # <major>[.<minor>[.<patch>[.<tweak>]]]]
#        DESCRIPTION <project-description-string>
        LANGUAGES C CXX
        )

enable_testing()

# Add a subdirectory to the build
# source_dir: CMakeLists.txt and code files are located
# binary_dir: place the output files, default to <source_dir>
add_subdirectory(test)
