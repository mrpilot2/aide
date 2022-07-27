cmake_minimum_required(VERSION 3.18...3.23)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(AideTestStandardProjectSettings)
standard_project_settings()

project(
  aide_tests
  LANGUAGES CXX
  VERSION 0.1.0
)

include(dependencies/CMakeLists.txt)

# In standalone mode we need to find our library
if(NOT TARGET Aide::AideApplication)
  find_package(Aide CONFIG REQUIRED)
  set(TEST_AGAINST_INSTALLED TRUE)
else()
  set(TEST_AGAINST_INSTALLED FALSE)
endif()

# target does not exist in standalone mode, only when built from top-level
if(NOT TARGET aide_project_sanitizers)
  add_library(aide_project_sanitizers INTERFACE IMPORTED)
endif()

# target does not exist in standalone mode, only when built from top-level
if(NOT TARGET aide_project_coverage)
  add_library(aide_project_coverage INTERFACE IMPORTED)
endif()

# target does not exist in standalone mode, only when built from top-level
if(NOT TARGET aide_project_warnings)
  add_library(aide_project_warnings INTERFACE IMPORTED)
endif()

enable_testing()
add_subdirectory(ut)
