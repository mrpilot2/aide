
FIND_PACKAGE(Qt5 COMPONENTS Core Widgets)

ADD_SUBDIRECTORY(commandmanager)

ADD_LIBRARY(api STATIC "")

TARGET_SOURCES(api
  PRIVATE
    id.cpp
  )

TARGET_LINK_LIBRARIES(api
  PUBLIC
    commandmanager
)

TARGET_INCLUDE_DIRECTORIES(api
  INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include/>
  $<INSTALL_INTERFACE:include/>
  PRIVATE
  ${CMAKE_SOURCE_DIR}/src/include/aide
)

TARGET_COMPILE_FEATURES(api
  PUBLIC
  cxx_std_14
)

IF(ENABLE_COVERAGE)
  TARGET_COMPILE_OPTIONS(api PRIVATE -g -O0 --coverage -fno-inline)
  TARGET_LINK_LIBRARIES(api PRIVATE --coverage)
ENDIF()

ADD_LIBRARY(aide SHARED)
ADD_LIBRARY(Aide::Aide ALIAS aide)

TARGET_SOURCES(aide
  PRIVATE
    application.cpp
)

TARGET_INCLUDE_DIRECTORIES(aide
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include/>
    $<INSTALL_INTERFACE:include/>
  PRIVATE
    include/aide
)

TARGET_LINK_LIBRARIES(aide
  PUBLIC
    Qt5::Widgets
    api
)

TARGET_COMPILE_FEATURES(aide
  PRIVATE
    cxx_generalized_initializers
  PUBLIC
    cxx_trailing_return_types
)

IF(ENABLE_COVERAGE)
  TARGET_COMPILE_OPTIONS(aide PRIVATE -g -O0 --coverage)
  TARGET_LINK_LIBRARIES(aide PRIVATE --coverage)
ENDIF()
