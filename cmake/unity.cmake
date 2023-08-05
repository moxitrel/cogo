include_guard(DIRECTORY)
include(FetchContent)
include(FindPackageMessage)

FetchContent_Declare(unity
  GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
  GIT_TAG v2.5.2
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(unity)

if(NOT DEFINED unity_VERSION)
  set(unity_VERSION v2.5.2)
endif()

find_package_message(unity
  "Found unity: ${unity_VERSION}"
  "${unity_VERSION}"
)
