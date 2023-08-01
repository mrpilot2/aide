cmake_minimum_required(VERSION 3.21...3.26)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(StandardProjectSettings)
standard_project_settings()

project(
  aide_demo
  LANGUAGES CXX
  VERSION 0.1.0
)

if(NOT TARGET Aide::AideApplication)
  find_package(Aide 0.1 REQUIRED)
endif()

find_package(
  Qt6
  COMPONENTS Core Widgets
  QUIET
)

if(TARGET Qt6::Core)
  find_package(Qt6LinguistTools)
else()
  find_package(
    Qt5
    COMPONENTS Core Widgets
    REQUIRED
  )

  if(NOT TARGET Qt::Core)
    add_library(Qt::Core ALIAS Qt5::Core)
    add_library(Qt::Widgets ALIAS Qt5::Widgets)
  endif()

  find_package(Qt5LinguistTools)
  add_executable(Qt::lrelease ALIAS Qt5::lrelease)
  add_executable(Qt::lupdate ALIAS Qt5::lupdate)
endif()

add_subdirectory(src)
