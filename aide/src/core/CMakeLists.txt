set(CMAKE_AUTOMOC ON)

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
          settings/settingsdialogchangepagecontroller.cpp
          settings/settingsdialoggeometryandstate.cpp
          settings/settingspage.cpp
          settings/settingspagegrouptreemodel.cpp
          settings/settingspagegrouptreemodel.hpp
          settings/settingspageregistry.cpp
          settings/showsettingsdialog.cpp
          settings/keymap/keymappage.cpp
          treeitem.cpp
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
  PRIVATE aide_project_coverage aide_project_sanitizers aide_project_warnings
)

target_compile_features(AideCore PRIVATE cxx_std_17)
