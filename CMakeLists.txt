cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/aide/cmake)

include(StandardProjectSettings)
standard_project_settings()

project(
  aide
  LANGUAGES CXX
  VERSION 0.1.0
)

include(CompilerWarnings)
include(StaticAnalyzers)
include(Sanitizers)

add_subdirectory(aide)

enable_testing()
add_subdirectory(tests)

option(ENABLE_DEMO "Builds a small demo application" ON)

if(ENABLE_DEMO)
  add_subdirectory(demo)
endif()
