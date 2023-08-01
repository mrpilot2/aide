cmake_minimum_required(VERSION 3.21...3.26)

include(FeatureSummary)

include(cmake/VersionFromGit.cmake)

version_from_git()

project(
  aide
  LANGUAGES CXX
  VERSION ${VERSION}
)

set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)

aide_setup_options()

aide_global_options()

aide_local_options()

add_subdirectory(aide)

enable_testing()
add_subdirectory(tests)

if(PROJECT_IS_TOP_LEVEL)

  option(aide_ENABLE_DEMO "Builds a small demo application" ON)
  add_feature_info(EnableDemo aide_ENABLE_DEMO "Builds the demo application.")

  if(aide_ENABLE_DEMO)
    add_subdirectory(demo)
  endif()

endif()

feature_summary(WHAT ALL)
