# automatically enable catch2 to generate ctest targets
include(Catch)

option(UNIT_TESTS_RUN_ON_CI
       "Specify if unit tests are executed on CI - changes reporter"
)

add_library(aide_catch_main STATIC catch_main.cpp)
target_link_libraries(aide_catch_main PUBLIC Catch2::Catch2)
target_compile_features(aide_catch_main PUBLIC cxx_std_11)

set(CMAKE_AUTORCC ON)

if(TARGET Qt5::lrelease)
  set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM TRUE)

  qt5_create_translation(
    QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} res/ut_sv.ts OPTIONS -locations none
  )

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
    mocksettingspage.cpp
    searchlineedittest.cpp
    settingspageregistrytest.cpp
    settingspagetest.cpp
    $<$<TARGET_EXISTS:Qt5::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/ut_translations.qrc
    ${QM_FILES}>
    res/ut_icons.qrc
)

if(NOT ${TEST_AGAINST_INSTALLED})
  target_sources(
    ut.aide
    PRIVATE actionregistrytest.cpp
            aidesettingsprovidertest.cpp
            applicationcloseinteractortest.cpp
            applicationtranslatortest.cpp
            loggertest.cpp
            mainwindowgeometryandstatetest.cpp
            mainwindowtest.cpp
            mockapplicationcloseview.cpp
            mockmainwindowview.cpp
            mocksettings.cpp
            mocksettingsdialog.cpp
            nulllogger.cpp
            qtsettingstest.cpp
            settingsdialoggeometryandstatetest.cpp
            settingspagegrouptreemodeltest.cpp
            showsettingsdialogtest.cpp
            treeitemtest.cpp
  )
endif()

target_link_libraries(
  ut.aide
  PRIVATE aide_catch_main
          Qt5::Core
          Qt5::Widgets
          Qt5::Test
          aide_project_sanitizers
          aide_project_warnings
          Aide::AideBuildInformation
          Aide::AideApplication
)

if(NOT ${TEST_AGAINST_INSTALLED})
  target_link_libraries(
    ut.aide PRIVATE Aide::AideCore Aide::AideGui Aide::AideLogger
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
  )
else()
  catch_discover_tests(ut.aide REPORTER console)
endif()
