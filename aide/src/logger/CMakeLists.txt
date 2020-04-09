add_library(aide_logger STATIC)
add_library(Aide::Logger ALIAS aide_logger)

target_sources(aide_logger PRIVATE logger.cpp)

target_include_directories(
  aide_logger
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  aide_logger
  PUBLIC CONAN_PKG::spdlog
  PRIVATE project_coverage
)

target_compile_definitions(
  aide_logger PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)
target_compile_features(aide_logger PUBLIC cxx_std_17)
