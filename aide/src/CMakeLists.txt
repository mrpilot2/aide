add_subdirectory(buildinformation)
add_subdirectory(logger)

add_library(aide_core SHARED)
add_library(Aide::AideCore ALIAS aide_core)

target_sources(aide_core PRIVATE application.cpp $<TARGET_OBJECTS:aide_logger>)

target_include_directories(
  aide_core
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/aide/>
)

target_link_libraries(
  aide_core
  PUBLIC CONAN_PKG::spdlog
  PRIVATE aide::project_coverage aide::project_sanitizers
)

target_compile_features(aide_core PUBLIC cxx_std_17)

target_compile_definitions(
  aide_core PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)
