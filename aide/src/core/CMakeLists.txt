add_library(AideCore OBJECT)
add_library(Aide::AideCore ALIAS AideCore)

target_sources(
  AideCore
  PRIVATE aidesettingsprovider.cpp
          actionregistry.cpp
          applicationclose.cpp
          hierarchicalid.cpp
          mainwindowgeometryandstate.cpp
          mainwindowinterface.cpp
          qtsettings.cpp
          settingsinterface.cpp
          settingsproviderinterface.cpp
)

target_include_directories(
  AideCore
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  AideCore
  PUBLIC Qt5::Core Qt5::Widgets AideLogger
  PRIVATE project_coverage project_sanitizers project_warnings
)

target_compile_features(AideCore PRIVATE cxx_std_17)
