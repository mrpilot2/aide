add_library(AideApplication OBJECT)
add_library(Aide::AideApplication ALIAS AideApplication)

target_sources(
  AideApplication
  PRIVATE aidesettingsprovider.cpp
          actionregistry.cpp
          application.cpp
          hierarchicalid.cpp
          qtsettings.cpp
          settingsinterface.cpp
          settingsproviderinterface.cpp
)

target_include_directories(
  AideApplication
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  AideApplication
  PUBLIC Qt5::Core Qt5::Widgets AideLogger
  PRIVATE AideGui project_coverage project_sanitizers project_warnings
)

target_compile_features(AideApplication PRIVATE cxx_std_17)
