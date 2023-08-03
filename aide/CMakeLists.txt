cmake_minimum_required(VERSION 3.21...3.26)

set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_CURRENT_LIST_DIR}/../cmake/VersionFromGit.cmake)

version_from_git()

project(
  aide
  LANGUAGES CXX
  VERSION ${VERSION}
)

if(PROJECT_IS_TOP_LEVEL)

  include(${CMAKE_CURRENT_LIST_DIR}/../cmake/PreventInSourceBuilds.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/../ProjectOptions.cmake)

  aide_setup_options()

  aide_global_options()

  aide_local_options()
endif()

include(dependencies/Qt.cmake)
add_subdirectory(dependencies)

add_subdirectory(doc)

add_subdirectory(src)

if(aide_ENABLE_ABI_COMPLIANCE_CHECK)
  include(cmake/AbiComplianceCheck.cmake)
  aide_perform_abi_compliance_check(Aide::AideApplication Aide::AideWidgets)
endif()

add_subdirectory(packaging)
