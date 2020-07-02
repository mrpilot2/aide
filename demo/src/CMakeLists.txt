set(CMAKE_AUTORCC ON)

if(TARGET Qt5::lrelease)
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
  main.cpp
  $<$<TARGET_EXISTS:Qt5::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/demo_translations.qrc
  ${QM_FILES}>
)

target_link_libraries(aide_demo PRIVATE Aide::AideCore)
