add_library(AideLogger OBJECT)
add_library(Aide::AideLogger ALIAS AideLogger)

target_sources(AideLogger PRIVATE logger.cpp)

target_include_directories(
  AideLogger
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  AideLogger
  PUBLIC CONAN_PKG::spdlog
  PRIVATE project_coverage project_sanitizers project_warnings
)

set_target_properties(AideLogger PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_compile_definitions(
  AideLogger PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)
target_compile_features(AideLogger PUBLIC cxx_std_17)
