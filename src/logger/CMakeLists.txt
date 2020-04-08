add_library(logger STATIC)

target_sources(logger PRIVATE logger.cpp)

target_include_directories(
  logger
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  logger
  PUBLIC CONAN_PKG::spdlog
  PRIVATE project_coverage
)

target_compile_definitions(logger PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)
target_compile_features(logger PUBLIC cxx_std_17)
