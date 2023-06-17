cmake_minimum_required(VERSION 3.21...3.26)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/aide/cmake)

include(FeatureSummary)

include(AideStandardProjectSettings)
standard_project_settings()

include(VersionFromGit)

version_from_git()

project(
  aide
  LANGUAGES CXX
  VERSION ${VERSION}
)

include(AideCompilerWarnings)
include(AideStaticAnalyzers)
include(AideSanitizers)

add_subdirectory(aide)

enable_testing()
add_subdirectory(tests)

option(ENABLE_DEMO "Builds a small demo application" ON)
add_feature_info(EnableDemo ENABLE_DEMO "Builds the demo application.")

if(ENABLE_DEMO)
  add_subdirectory(demo)
endif()

feature_summary(WHAT ALL)
