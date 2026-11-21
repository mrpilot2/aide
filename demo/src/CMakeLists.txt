set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS "res")
set(CMAKE_AUTOMOC ON)

if(TARGET Qt::lrelease)
  set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)
  qt_create_translation(
    QM_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}
    res/demo_de.ts
    res/demo_en.ts
    res/demo_zh_CN.ts
    res/demo_hi_IN.ts
    res/demo_es_ES.ts
    res/demo_fr_FR.ts
    res/demo_ar_SA.ts
    OPTIONS
    -locations
    none
  )

  configure_file(
    res/demo_translations.qrc.in
    ${CMAKE_CURRENT_BINARY_DIR}/demo_translations.qrc
  )
endif()

# Everything but main(), so the demo tests (demo/tests) link the same code
# the demo app runs instead of recompiling its sources.
add_library(
  aide_demo_lib
  STATIC
  colorschemereactor.hpp
  demosettingspage.cpp
  demosetup.cpp
  desktopurllauncher.cpp
  notificationdemodialog.cpp
  notificationlauncherdialog.cpp
  reportbugpreviewlauncher.cpp
)

target_include_directories(aide_demo_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  aide_demo_lib
  PUBLIC
    Aide::AideApplication
    Aide::AideCore
    Aide::AideWidgets
    Qt::Core
    Qt::Widgets
  PRIVATE aide_warnings aide_options
)

add_executable(
  aide_demo
  $<$<TARGET_EXISTS:Qt::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/demo_translations.qrc
  ${QM_FILES}>
  res/icons/demo_icons.qrc
  main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../../tools/sanitizerdefaults.cpp
)

target_link_libraries(
  aide_demo
  PRIVATE aide_demo_lib aide_warnings aide_options
)

if(WIN32)
  target_compile_definitions(
    aide_demo_lib
    PUBLIC _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
  )
endif()

if(APPLE)
  add_custom_command(
    TARGET aide_demo
    POST_BUILD
    COMMAND
      ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${Qt6_DIR}/../../"
      $<TARGET_FILE:aide_demo>
  )
endif()
