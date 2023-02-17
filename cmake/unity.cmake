include_guard(DIRECTORY)
include(FetchContent)
include(FindPackageMessage)

unset(_PackageName)
unset(_PackageUrl)
unset(_PackageVersion)
set(_PackageName unity)
set(_PackageUrl https://github.com/ThrowTheSwitch/Unity.git)
set(_PackageVersion v2.5.2)

FetchContent_Declare(${_PackageName}
    GIT_REPOSITORY ${_PackageUrl}
    GIT_TAG ${_PackageVersion}
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(${_PackageName})

if(NOT DEFINED ${_PackageName}_VERSION)
    set(${_PackageName}_VERSION ${_PackageVersion})
endif()

link_libraries(
    ${_PackageName}
)

if($<TARGET_EXISTS:pch>)
    target_precompile_headers(pch INTERFACE
        <unity.h>
    )
endif()

find_package_message(${_PackageName}
    "Found ${_PackageName}: ${${_PackageName}_VERSION}"
    "${${_PackageName}_VERSION}"
)

unset(_PackageName)
unset(_PackageUrl)
unset(_PackageVersion)
