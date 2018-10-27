ADD_LIBRARY(commandmanager STATIC "")

TARGET_SOURCES(commandmanager
  PRIVATE
    commandmanager.cpp
)

TARGET_LINK_LIBRARIES(commandmanager
  PUBLIC
    Qt5::Core
    Qt5::Widgets
)

TARGET_INCLUDE_DIRECTORIES(commandmanager
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include/>
    $<INSTALL_INTERFACE:include/>
  PRIVATE
  ../../include/aide/commandmanager
)

IF(ENABLE_COVERAGE)
  TARGET_COMPILE_OPTIONS(commandmanager PRIVATE -g -O0 --coverage -fno-inline)
  TARGET_LINK_LIBRARIES(commandmanager PRIVATE --coverage)
ENDIF()