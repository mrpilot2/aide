add_subdirectory(buildinformation)
add_subdirectory(logger)

add_library(aide SHARED)

target_sources(aide PRIVATE application.cpp)

target_include_directories(
  aide
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/aide/>
)

target_link_libraries(
  aide PRIVATE aide::project_coverage aide::project_sanitizers
)

target_compile_features(aide PUBLIC cxx_std_11)
