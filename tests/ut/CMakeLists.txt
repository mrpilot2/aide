# automatically enable catch2 to generate ctest targets
include(Catch)

option(UNIT_TESTS_RUN_ON_CI
       "Specify if unit tests are executed on CI - changes reporter"
)

add_library(aide_catch_main STATIC catch_main.cpp)
target_link_libraries(
  aide_catch_main
  PUBLIC Catch2::Catch2
  PRIVATE Aide::AideApplication
)
target_compile_features(aide_catch_main PUBLIC cxx_std_11)

set(CMAKE_AUTORCC ON)

if(TARGET Qt::lrelease)
  set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)

  if(Qt6Core_VERSION_MAJOR)
    qt_create_translation(
      QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} res/ut_sv.ts OPTIONS -locations none
    )
  else()
    qt5_create_translation(
      QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} res/ut_sv.ts OPTIONS -locations none
    )
  endif()

  configure_file(
    res/ut_translations.qrc.in ${CMAKE_CURRENT_BINARY_DIR}/ut_translations.qrc
  )
endif()

add_executable(ut.aide)

target_sources(
  ut.aide
  PRIVATE
    applicationtest.cpp
    buildinformationtest.cpp
    hierarchicalidtest.cpp
    mocksettings.cpp
    mocksettingspage.cpp
    searchfilterwidgettest.cpp
    settingspageregistrytest.cpp
    settingspagetest.cpp
    $<$<TARGET_EXISTS:Qt::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/ut_translations.qrc
    ${QM_FILES}>
    res/ut_icons.qrc
)

if(NOT PROJECT_IS_TOP_LEVEL)
  target_sources(
    ut.aide
    PRIVATE aboutaideusecasetest.cpp
            actionregistrytest.cpp
            aidesettingsprovidertest.cpp
            applicationcloseinteractortest.cpp
            $<$<TARGET_EXISTS:Qt::lrelease>:applicationtranslatortest.cpp>
            keymapcontextmenuprovidertest.cpp
            keymaptreemodeltest.cpp
            keymappagetest.cpp
            loggertest.cpp
            mainwindowgeometryandstatetest.cpp
            mainwindowtest.cpp
            menucontainertest.cpp
            mockaboutdialog.cpp
            mockapplicationcloseview.cpp
            mockkeymappagewidget.cpp
            mockmainwindowview.cpp
            mocksettingsdialog.cpp
            nulllogger.cpp
            qtsettingstest.cpp
            settingsdialoggeometryandstatetest.cpp
            settingspagegrouptreemodeltest.cpp
            showkeymaptest.cpp
            showsettingsdialogtest.cpp
            systemmemorytest.cpp
            treeitemtest.cpp
  )
endif()

target_link_libraries(
  ut.aide
  PRIVATE aide_catch_main
          Qt::Core
          Qt::Widgets
          Qt::Test
          aide_coverage
          aide_options
          aide_warnings
          Aide::AideBuildInformation
          Aide::AideApplication
          Aide::AideWidgets
)

if(NOT PROJECT_IS_TOP_LEVEL)
  target_link_libraries(
    ut.aide PRIVATE Aide::AideCore Aide::AideGui Aide::AideLogger
                    Aide::AideUtils
  )
  target_include_directories(
    ut.aide PRIVATE ${CMAKE_SOURCE_DIR}/aide/src/include
                    ${CMAKE_SOURCE_DIR}/aide/src/include/aide
  )
endif()

if(APPLE)
  add_custom_command(
    TARGET ut.aide
    POST_BUILD
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${Qt5_DIR}/../../"
            $<TARGET_FILE:ut.aide>
  )
endif()

set(LICENSE_FILE "${CMAKE_INSTALL_PREFIX}/share/doc/aide/licenses.html")
set(DEVEL_LICENSE_FILE
    "${CMAKE_CURRENT_BINARY_DIR}/../../aide/dependencies/licenses.html"
)

set_source_files_properties(
  aboutaideusecasetest.cpp
  PROPERTIES
    COMPILE_DEFINITIONS
    "INSTALL_LICENSE_FILE=\"${LICENSE_FILE}\";DEVEL_LICENSE_FILE=\"${DEVEL_LICENSE_FILE}\""
)

enable_testing()

if(UNIT_TESTS_RUN_ON_CI)
  catch_discover_tests(
    ut.aide
    REPORTER
    junit
    OUTPUT_DIR
    ${CMAKE_CURRENT_BINARY_DIR}
    OUTPUT_SUFFIX
    .xml
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}/../../
  )
else()
  catch_discover_tests(
    ut.aide REPORTER console WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/../../
  )
endif()

set_source_files_properties(
  loggertest.cpp
  PROPERTIES COMPILE_DEFINITIONS
             TEST_LOG_FILE_LOCATION="${CMAKE_CURRENT_BINARY_DIR}"
)
