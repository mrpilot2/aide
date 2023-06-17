set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/res/forms)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(widgets)

if(TARGET Qt::lrelease)
  set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)

  if(Qt6Core_VERSION_MAJOR)
    qt_create_translation(
      QM_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/../core
      res/translations/aide_de_DE.ts
      res/translations/aide_en.ts
      OPTIONS
      -locations
      none
    )
  else()
    qt5_create_translation(
      QM_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/../core
      res/translations/aide_de_DE.ts
      res/translations/aide_en.ts
      OPTIONS
      -locations
      none
    )
  endif()

  configure_file(
    res/translations/translations.qrc.in
    ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
  )
endif()

add_library(AideGui OBJECT)
add_library(Aide::AideGui ALIAS AideGui)

target_sources(
  AideGui
  PRIVATE
    aboutaidedialog.cpp
    mainwindow.cpp
    mainwindowcontroller.cpp
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

target_include_directories(
  AideGui
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include/aide/gui
)

target_compile_features(AideGui PUBLIC cxx_std_17)

target_link_libraries(
  AideGui
  PRIVATE Aide::AideCore
          Aide::AideLogger
          Aide::AideWidgets
          Aide::AideUtils
          Qt::Widgets
          aide_project_sanitizers
          aide_project_warnings
)

set_source_files_properties(
  ${CMAKE_CURRENT_BINARY_DIR}/AideGui_autogen/mocs_compilation.cpp
  PROPERTIES COMPILE_FLAGS "$<$<CXX_COMPILER_ID:GNU>:-Wno-useless-cast>"
)
