cmake_minimum_required(VERSION 3.26)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake)

include(VersionFromGit)

version_from_git()

project(aide.library LANGUAGES CXX VERSION ${VERSION})

set(CMAKE_CXX_EXTENSIONS OFF)

include(CheckRequiredCXXStandard)

if(PROJECT_IS_TOP_LEVEL)
  include(PreventInSourceBuilds)
  include(${CMAKE_CURRENT_LIST_DIR}/../ProjectOptions.cmake)

  aide_setup_options()

  aide_global_options()

  aide_local_options()
endif()

include(FeatureSummary)

option(
  AIDE_ENABLE_VCS
  "Builds the aide::vcs Git version control integration module"
  ON
)
add_feature_info(
  EnableVcs
  AIDE_ENABLE_VCS
  "Builds the aide::vcs Git version control integration module."
)

include(dependencies/Qt.cmake)
add_subdirectory(dependencies)

add_subdirectory(doc)

add_subdirectory(src)

if(aide_ENABLE_ABI_COMPLIANCE_CHECK)
  include(AbiComplianceCheck)
  aide_perform_abi_compliance_check(Aide::AideApplication Aide::AideWidgets)
endif()

add_subdirectory(packaging)
