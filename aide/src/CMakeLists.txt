add_subdirectory(buildinformation)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(logger)

add_library(AideApplication)
add_library(Aide::AideApplication ALIAS AideApplication)

target_sources(
  AideApplication
  PRIVATE $<TARGET_OBJECTS:AideCore> $<TARGET_OBJECTS:AideGui>
          $<TARGET_OBJECTS:AideWidgets> $<TARGET_OBJECTS:AideLogger>
          application.cpp
)

target_include_directories(
  AideApplication
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  PUBLIC $<INSTALL_INTERFACE:include/aide/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/aide/>
)

target_link_libraries(
  AideApplication
  PUBLIC CONAN_PKG::spdlog Qt5::Core Qt5::Widgets
  PRIVATE Aide::AideCore Aide::AideGui project_coverage project_sanitizers
)

target_compile_features(AideApplication PUBLIC cxx_std_17)

target_compile_definitions(
  AideApplication PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)
