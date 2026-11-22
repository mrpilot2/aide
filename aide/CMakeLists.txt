cmake_minimum_required(VERSION 3.26)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake)

project(aide.library LANGUAGES CXX VERSION 0.3.1) # x-release-please-version

set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckRequiredCXXStandard)

if(PROJECT_IS_TOP_LEVEL)
  include(PreventInSourceBuilds)
  include(${CMAKE_CURRENT_LIST_DIR}/../ProjectOptions.cmake)

  aide_setup_options()

  aide_global_options()

  aide_local_options()
endif()

# End-User Help pages for aIDE's built-in Help Topics. A cache entry, so
# FetchContent / add_subdirectory consumers see it from any directory scope;
# the installed package config sets the same variable for find_package users.
set(
  AIDE_HELP_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}/help"
  CACHE INTERNAL
  "Directory holding aIDE's End-User Help pages"
)

include(dependencies/Qt.cmake)
add_subdirectory(dependencies)

add_subdirectory(doc)

add_subdirectory(src)

if(AIDE_ENABLE_ABI_COMPLIANCE_CHECK)
  include(AbiComplianceCheck)
  aide_perform_abi_compliance_check(Aide::AideApplication Aide::AideWidgets)
endif()

add_subdirectory(packaging)
