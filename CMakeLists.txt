cmake_minimum_required(VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

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

include(dependencies/CMakeLists.txt)

enable_testing()
add_subdirectory(tests)

add_subdirectory(doc)

add_subdirectory(src)
