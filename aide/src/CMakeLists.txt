add_subdirectory(application)
add_subdirectory(buildinformation)
add_subdirectory(logger)

add_library(AideCore)
add_library(Aide::AideCore ALIAS AideCore)

target_sources(
  AideCore PRIVATE $<TARGET_OBJECTS:AideApplication>
                   $<TARGET_OBJECTS:AideLogger>
)

target_include_directories(
  AideCore
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/aide/>
)

target_link_libraries(
  AideCore
  PUBLIC CONAN_PKG::spdlog
  PRIVATE project_coverage project_sanitizers
)

target_compile_features(AideCore PUBLIC cxx_std_17)

target_compile_definitions(
  AideCore PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)
