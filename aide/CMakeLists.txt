cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(StandardProjectSettings)
standard_project_settings()

project(
  aide
  LANGUAGES CXX
  VERSION 0.1.0
)

if(NOT TARGET project_sanitizers)
  add_library(project_sanitizers INTERFACE)
endif()

if(NOT TARGET project_coverage)
  add_library(project_coverage INTERFACE)
endif()

if(NOT TARGET project_warnings)
  add_library(project_warnings INTERFACE)
endif()

include(dependencies/CMakeLists.txt)

add_subdirectory(doc)

add_subdirectory(src)

add_subdirectory(packaging)
