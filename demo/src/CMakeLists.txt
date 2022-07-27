set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "res")
set(CMAKE_AUTOMOC ON)

if(TARGET Qt::lrelease)
  set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)
  qt5_create_translation(
    QM_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}
    res/demo_de.ts
    res/demo_en.ts
    OPTIONS
    -locations
    none
  )

  configure_file(
    res/demo_translations.qrc.in
    ${CMAKE_CURRENT_BINARY_DIR}/demo_translations.qrc
  )
endif()

add_executable(
  aide_demo
  $<$<TARGET_EXISTS:Qt::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/demo_translations.qrc
  ${QM_FILES}>
  demosettingspage.cpp
  main.cpp
)

target_link_libraries(aide_demo PRIVATE Aide::AideApplication)

if(APPLE)
  add_custom_command(
    TARGET aide_demo
    POST_BUILD
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${Qt5_DIR}/../../"
            $<TARGET_FILE:aide_demo>
  )
endif()
