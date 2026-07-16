set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/res/forms)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(widgets)

if(TARGET Qt::lrelease)
  set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)

  qt_create_translation(
    QM_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    res/translations/aide_de_DE.ts
    res/translations/aide_en.ts
    res/translations/aide_zh_CN.ts
    res/translations/aide_hi_IN.ts
    res/translations/aide_es_ES.ts
    res/translations/aide_fr_FR.ts
    res/translations/aide_ar_SA.ts
    OPTIONS
    -locations
    none
  )

  configure_file(
    res/translations/translations.qrc.in
    ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
  )
endif()

add_library(AideGui)
add_library(Aide::AideGui ALIAS AideGui)

target_sources(
  AideGui
  PRIVATE
    aboutaidedialog.cpp
    mainwindow.cpp
    mainwindowcontroller.cpp
    thirdpartylicensesdialog.cpp
    settings/appearancepage.cpp
    settings/changedetector.cpp
    settings/settingsdialog.cpp
    settings/settingsdialogcontroller.cpp
    settings/keymap/addshortcutdialog.cpp
    settings/keymap/keymappagewidget.cpp
    settings/keymap/keymappagewidgetcontroller.cpp
    $<$<TARGET_EXISTS:Qt::lrelease>:applicationtranslator.cpp>
    $<$<NOT:$<TARGET_EXISTS:Qt::lrelease>>:nullapplicationtranslator.cpp>
    $<$<TARGET_EXISTS:Qt::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    ${QM_FILES}>
    res/icons/icons.qrc
)

target_sources(
  AideGui
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include
      FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/gui/translatorinterface.hpp
)

target_include_directories(
  AideGui
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include/aide/gui
)

target_link_libraries(
  AideGui
  PRIVATE
    Aide::AideCore
    Aide::AideLogger
    Aide::AideWidgets
    Aide::AideUtils
    Qt::Widgets
    $<BUILD_LOCAL_INTERFACE:aide_options>
    $<BUILD_LOCAL_INTERFACE:aide_warnings>
    $<BUILD_LOCAL_INTERFACE:aide_coverage>
)

set_source_files_properties(
  ${CMAKE_CURRENT_BINARY_DIR}/AideGui_autogen/mocs_compilation.cpp
  PROPERTIES COMPILE_FLAGS "$<$<CXX_COMPILER_ID:GNU>:-Wno-useless-cast>"
)
