include_guard(DIRECTORY)
include(FetchContent)
include(FindPackageMessage)

unset(_PackageName)
set(_PackageName unity)

FetchContent_Declare(${_PackageName}
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    GIT_TAG v2.5.2
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(${_PackageName})
if(NOT DEFINED ${_PackageName}_VERSION)
    set(${_PackageName}_VERSION 2.5.2)
endif()

link_libraries(
    unity
)
if ($<TARGET_EXISTS:pch>)
    target_precompile_headers(pch INTERFACE
        <unity.h>
    )
endif()

find_package_message(${_PackageName}
    "Found ${_PackageName}: ${${_PackageName}_VERSION}"
    "${${_PackageName}_VERSION}"
)

unset(_PackageName)
