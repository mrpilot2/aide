set(CMAKE_AUTOMOC ON)

add_library(AideCore OBJECT)
add_library(Aide::AideCore ALIAS AideCore)

target_sources(
  AideCore
  PRIVATE aboutaideusecase.cpp
          aidesettingsprovider.cpp
          actionregistry.cpp
          applicationclose.cpp
          hierarchicalid.cpp
          loggerfactory.cpp
          mainwindowgeometryandstate.cpp
          mainwindowinterface.cpp
          menucontainer.cpp
          menucontainerinterface.cpp
          ../include/aide/menucontainerinterface.hpp
          qtsettings.cpp
          settingsinterface.cpp
          settings/settingsdialogchangepagecontroller.cpp
          settings/settingsdialoggeometryandstate.cpp
          settings/settingspage.cpp
          settings/settingspagegrouptreemodel.cpp
          settings/settingspagegrouptreemodel.hpp
          settings/settingspageregistry.cpp
          settings/showsettingsdialog.cpp
          settings/keymap/keymapcontextmenuprovider.cpp
          settings/keymap/keymappage.cpp
          settings/keymap/keymaptreemodel.cpp
          settings/keymap/showkeymap.cpp
          treeitem.cpp
          treemodel.cpp
)

target_include_directories(
  AideCore
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  AideCore
  PUBLIC Qt::Core Qt::Widgets AideLogger
  PRIVATE Aide::AideBuildInformation aide_coverage aide_options aide_warnings
)

target_compile_features(AideCore PRIVATE cxx_std_17)

set(LICENSE_FILE "${CMAKE_INSTALL_PREFIX}/share/doc/aide/licenses.html")
set(DEVEL_LICENSE_FILE "${CMAKE_BINARY_DIR}/aide/dependencies/licenses.html")

set_source_files_properties(
  aboutaideusecase.cpp
  PROPERTIES
    COMPILE_DEFINITIONS
    "INSTALL_LICENSE_FILE=\"${LICENSE_FILE}\";DEVEL_LICENSE_FILE=\"${DEVEL_LICENSE_FILE}\""
)
