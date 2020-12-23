cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(StandardProjectSettings)
standard_project_settings()

project(
  aide_demo
  LANGUAGES CXX
  VERSION 0.1.0
)

if(NOT TARGET project_sanitizers)
  add_library(project_sanitizers INTERFACE)
endif()

if(NOT TARGET project_coverage)
  add_library(project_coverage INTERFACE)
endif()

if(NOT TARGET Aide::AideApplication)
  find_package(Aide 0.1 REQUIRED)
endif()

find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5LinguistTools)

add_subdirectory(src)
