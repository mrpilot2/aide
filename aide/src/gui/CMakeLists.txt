set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/res/forms)
set(CMAKE_AUTORCC ON)

if(TARGET Qt5::lrelease)
  qt5_create_translation(
    QM_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}
    res/translations/aide_de.ts
    res/translations/aide_en.ts
    OPTIONS
    -locations
    none
  )

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
    mainwindow.cpp
    $<$<TARGET_EXISTS:Qt5::lrelease>:applicationtranslator.cpp>
    $<$<NOT:$<TARGET_EXISTS:Qt5::lrelease>>:nullapplicationtranslator.cpp>
    $<$<TARGET_EXISTS:Qt5::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/translations.qrc
    ${QM_FILES}>
)

target_include_directories(
  AideGui PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include
                  ${CMAKE_CURRENT_LIST_DIR}/../include/gui
)

target_compile_features(AideGui PUBLIC cxx_std_17)

target_link_libraries(
  AideGui PRIVATE Aide::AideLogger Qt5::Widgets project_coverage
)