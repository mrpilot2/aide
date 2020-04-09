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
endif()

# target does not exist in standalone mode, only when built from top-level
if(NOT TARGET aide::project_sanitizers)
  add_library(project_sanitizers INTERFACE)
  add_library(aide::project_sanitizers ALIAS project_sanitizers)
endif()

enable_testing()
add_subdirectory(ut)
