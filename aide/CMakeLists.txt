cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(AideStandardProjectSettings)
standard_project_settings()

project(
  aide
  LANGUAGES CXX
  VERSION 0.1.0
)

if(NOT TARGET aide_project_sanitizers)
  add_library(aide_project_sanitizers INTERFACE)
endif()

if(NOT TARGET aide_project_coverage)
  add_library(aide_project_coverage INTERFACE)
endif()

if(NOT TARGET aide_project_warnings)
  add_library(aide_project_warnings INTERFACE)
endif()

include(dependencies/Qt.cmake)
add_subdirectory(dependencies)

add_subdirectory(doc)

add_subdirectory(src)

add_subdirectory(packaging)
