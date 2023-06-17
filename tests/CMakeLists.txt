cmake_minimum_required(VERSION 3.21...3.26)

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
  find_package(AideQt${QT_VERSION_MAJOR} CONFIG REQUIRED)
endif()

enable_testing()
add_subdirectory(ut)
