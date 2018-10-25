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
)

TARGET_COMPILE_FEATURES(aide
  PRIVATE
    cxx_generalized_initializers
)

IF(ENABLE_COVERAGE)
  TARGET_COMPILE_OPTIONS(aide PRIVATE -g -O0 --coverage)
  TARGET_LINK_LIBRARIES(aide PRIVATE --coverage)
ENDIF()
