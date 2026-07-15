cmake_minimum_required(VERSION 3.26)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../cmake)

include(StandardProjectSettings)

project(aide_demo LANGUAGES CXX VERSION 0.1.0)

if(NOT TARGET Aide::AideApplication)
  find_package(Aide 0.1 REQUIRED)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

find_package(Qt6LinguistTools)

add_subdirectory(src)
