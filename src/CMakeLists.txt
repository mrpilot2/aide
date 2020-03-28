add_library(aide SHARED)

target_sources(aide PRIVATE application.cpp)

target_include_directories(
  aide
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/aide/>
)
