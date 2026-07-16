set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_library(AideCore)
add_library(Aide::AideCore ALIAS AideCore)

target_sources(
  AideCore
  PRIVATE
    aboutaideusecase.cpp
    aidesettingsprovider.cpp
    actionregistry.cpp
    appearancemanager.cpp
    ../applicationconfig.cpp
    # Icon theme resource lives with AppearanceManager, which activates
    # the aide-dark / aide-light themes and must be able to resolve
    # :/aide/icons regardless of whether the GUI layer is linked.
    ../gui/res/icons/icon_themes.qrc
    applicationclose.cpp
    desktopentrynameresolver.cpp
    hierarchicalid.cpp
    loggerfactory.cpp
    mainwindowgeometryandstate.cpp
    mainwindowinterface.cpp
    menucontainer.cpp
    menucontainerinterface.cpp
    osfilemanagerlauncher.cpp
    qtsettings.cpp
    settingsinterface.cpp
    showloginfilemanagerusecase.cpp
    settings/settingsdialogchangepagecontroller.cpp
    settings/settingsdialoggeometryandstate.cpp
    settings/settingspage.cpp
    settings/settingspagefilterproxymodel.cpp
    settings/settingspagefilterproxymodel.hpp
    settings/settingspagegrouptreemodel.cpp
    settings/settingspagegrouptreemodel.hpp
    settings/searchpattern.hpp
    settings/searchhistory.cpp
    settings/searchhistory.hpp
    settings/settingspageranker.cpp
    settings/settingspageranker.hpp
    settings/textmatcher.cpp
    settings/textmatcher.hpp
    settings/settingspageregistry.cpp
    settings/showsettingsdialog.cpp
    settings/keymap/keymapcontextmenuprovider.cpp
    settings/keymap/keymapcontroller.cpp
    settings/keymap/keymappage.cpp
    settings/keymap/keymaptreemodel.cpp
    settings/keymap/pendingkeymapstate.cpp
    treeitem.cpp
    treemodel.cpp
)

target_sources(
  AideCore
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include
      FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/actionregistryinterface.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/aideconstants.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/aidesettingsprovider.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/appearancemanager.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/applicationconfig.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/colorscheme.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/hierarchicalid.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/menucontainerinterface.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/settingsinterface.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/theme.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/settings/settingspage.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/settings/settingspageregistry.hpp
)

target_include_directories(
  AideCore
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  AideCore
  PUBLIC Qt::Core Qt::Widgets Aide::AideLogger
  PRIVATE
    Aide::AideBuildInformation
    $<BUILD_LOCAL_INTERFACE:aide_coverage>
    $<BUILD_LOCAL_INTERFACE:aide_options>
    $<BUILD_LOCAL_INTERFACE:aide_warnings>
)

if(WIN32)
  target_compile_definitions(
    AideCore
    PUBLIC _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
  )
endif()

set(LICENSE_FILE "${CMAKE_INSTALL_PREFIX}/share/doc/aide/licenses.html")
set(
  DEVEL_LICENSE_FILE
  "${CMAKE_CURRENT_BINARY_DIR}/../../dependencies/licenses.html"
)

set_source_files_properties(
  aboutaideusecase.cpp
  PROPERTIES
    COMPILE_DEFINITIONS
      "INSTALL_LICENSE_FILE=\"${LICENSE_FILE}\";DEVEL_LICENSE_FILE=\"${DEVEL_LICENSE_FILE}\""
)
