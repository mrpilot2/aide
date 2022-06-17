add_subdirectory(buildinformation)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(logger)

add_library(AideQt${QT_VERSION_MAJOR}Application)
add_library(Aide::AideApplication ALIAS AideQt${QT_VERSION_MAJOR}Application)

target_sources(
  AideQt${QT_VERSION_MAJOR}Application
  PRIVATE $<TARGET_OBJECTS:AideCore> $<TARGET_OBJECTS:AideGui>
          $<TARGET_OBJECTS:AideQt${QT_VERSION_MAJOR}Widgets>
          $<TARGET_OBJECTS:AideLogger> application.cpp applicationbuilder.cpp
)

target_include_directories(
  AideQt${QT_VERSION_MAJOR}Application
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
  PUBLIC $<INSTALL_INTERFACE:include/aide/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/aide/>
)

target_link_libraries(
  AideQt${QT_VERSION_MAJOR}Application
  PUBLIC spdlog::spdlog Qt::Core Qt::Widgets
  PRIVATE Aide::AideCore Aide::AideGui aide_project_coverage
          aide_project_sanitizers
)

target_compile_features(AideQt${QT_VERSION_MAJOR}Application PUBLIC cxx_std_17)

target_compile_definitions(
  AideQt${QT_VERSION_MAJOR}Application
  PUBLIC SPDLOG_ACTIVE_LEVEL=${AIDE_LOG_LEVEL}
)
