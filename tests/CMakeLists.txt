cmake_minimum_required(VERSION 3.24...3.26)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

project(
  aide_tests
  LANGUAGES CXX
  VERSION 0.1.0
)

if(PROJECT_IS_TOP_LEVEL)

  include(${CMAKE_CURRENT_LIST_DIR}/../cmake/PreventInSourceBuilds.cmake)
  include(${CMAKE_CURRENT_LIST_DIR}/../ProjectOptions.cmake)

  aide_setup_options()

  aide_global_options()

  aide_local_options()
endif()

include(dependencies/CMakeLists.txt)

# In standalone mode we need to find our library
if(NOT TARGET Aide::AideApplication)
  find_package(AideQt${QT_VERSION_MAJOR} CONFIG REQUIRED)
endif()

enable_testing()
add_subdirectory(ut)
