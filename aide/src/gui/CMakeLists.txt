set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/res/forms)
set(CMAKE_AUTORCC ON)

qt5_create_translation(
  QM_FILES
  ${CMAKE_SOURCE_DIR}
  res/translations/aide_de.ts
  OPTIONS
  -locations
  none
)

configure_file(
  res/translations/translations.qrc.in
  ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
)

add_library(AideGui OBJECT)
add_library(Aide::AideGui ALIAS AideGui)

target_sources(
  AideGui PRIVATE mainwindow.cpp ${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
                  ${QM_FILES}
)

target_include_directories(
  AideGui PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include
                  ${CMAKE_CURRENT_LIST_DIR}/../include/gui
)

target_link_libraries(AideGui PRIVATE Qt5::Widgets project_coverage)
