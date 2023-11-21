add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(logger)
add_subdirectory(sentry)

add_library(AideQt${QT_VERSION_MAJOR}Application)
add_library(Aide::AideApplication ALIAS AideQt${QT_VERSION_MAJOR}Application)

target_sources(
  AideQt${QT_VERSION_MAJOR}Application
  PRIVATE $<TARGET_OBJECTS:AideCore>
          $<TARGET_OBJECTS:AideGui>
          $<TARGET_OBJECTS:AideLogger>
          $<TARGET_OBJECTS:AideSentry>
          $<TARGET_OBJECTS:AideUtils>
          application.cpp
          applicationbuilder.cpp
          applicationconfig.cpp
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
  PRIVATE Aide::AideCore Aide::AideGui Aide::AideUtils aide_coverage
          aide_options aide_warnings
)

if(ENABLE_SENTRY_INTEGRATION)
  target_link_libraries(
    AideQt${QT_VERSION_MAJOR}Application
    PUBLIC sentry::sentry sentry-breakpad::sentry-breakpad
  )
endif()

target_compile_features(AideQt${QT_VERSION_MAJOR}Application PUBLIC cxx_std_17)

target_compile_definitions(
  AideQt${QT_VERSION_MAJOR}Application
  PUBLIC SPDLOG_ACTIVE_LEVEL=${AIDE_LOG_LEVEL}
)

if(MINIMAL_RUNTIME AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  target_compile_options(
    AideQt${QT_VERSION_MAJOR}Application PUBLIC -fsanitize-minimal-runtime
                                                -fsanitize=undefined
  )
  target_link_options(
    AideQt${QT_VERSION_MAJOR}Application PUBLIC -fsanitize-minimal-runtime
    -fsanitize=undefined
  )
endif()

add_subdirectory(buildinformation)
