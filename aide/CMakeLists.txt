cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(StandardProjectSettings)
standard_project_settings()

project(
  aide
  LANGUAGES CXX
  VERSION 0.1.0
)

if (NOT TARGET aide::project_sanitizers)
  add_library(project_sanitizers INTERFACE)
  add_library(aide::project_sanitizers ALIAS project_sanitizers)
endif()

if (NOT TARGET aide::project_coverage)
  add_library(project_coverage INTERFACE)
  add_library(aide::project_coverage ALIAS project_coverage)
endif()

include(dependencies/CMakeLists.txt)

add_subdirectory(doc)

add_subdirectory(src)
