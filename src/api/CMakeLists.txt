ADD_LIBRARY(aide_api "")

TARGET_SOURCES(aide_api
  PRIVATE
    id.cpp
)

TARGET_INCLUDE_DIRECTORIES(aide_api
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include/>
    $<INSTALL_INTERFACE:include/>
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include/aide
)

TARGET_COMPILE_FEATURES(aide_api
  PUBLIC
    cxx_std_14
)