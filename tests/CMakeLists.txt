cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(StandardProjectSettings)
standard_project_settings()

project(
  aide_tests
  LANGUAGES CXX
  VERSION 0.1.0
)

include(dependencies/CMakeLists.txt)

# In standalone mode we need to find our library
if(NOT TARGET Aide::AideCore)
  find_package(Aide CONFIG REQUIRED)
  set(TEST_AGAINST_INSTALLED TRUE)
else()
  set(TEST_AGAINST_INSTALLED FALSE)
endif()

# target does not exist in standalone mode, only when built from top-level
if(NOT TARGET project_sanitizers)
  add_library(project_sanitizers INTERFACE IMPORTED)
endif()

# target does not exist in standalone mode, only when built from top-level
if(NOT TARGET project_coverage)
  add_library(project_coverage INTERFACE IMPORTED)
endif()

# target does not exist in standalone mode, only when built from top-level
if(NOT TARGET project_warnings)
  add_library(project_warnings INTERFACE IMPORTED)
endif()

enable_testing()
add_subdirectory(ut)
