# automatically enable catch2 to generate ctest targets
include(Catch)

option(UNIT_TESTS_RUN_ON_CI
       "Specify if unit tests are executed on CI - changes reporter"
)

add_library(aide_catch_main STATIC catch_main.cpp)
target_link_libraries(
  aide_catch_main
  PUBLIC Catch2::Catch2
  PRIVATE Aide::AideApplication Qt::Widgets
)
target_compile_features(aide_catch_main PUBLIC cxx_std_11)

# Separate entry point for the [Application] tests: aide::Application is itself
# a QApplication, so those tests must not run alongside the process-global
# QApplication created in catch_main.cpp. This main deliberately creates none.
add_library(aide_catch_main_application STATIC catch_main_application.cpp)
target_link_libraries(
  aide_catch_main_application
  PUBLIC Catch2::Catch2
  PRIVATE Aide::AideApplication
)
target_compile_features(aide_catch_main_application PUBLIC cxx_std_11)

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
    aidetableviewtest.cpp
    aidetreeviewtest.cpp
    appearancemanagertest.cpp
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
            addshortcutdialogtest.cpp
            appearancepagetest.cpp
            aidesettingsprovidertest.cpp
            applicationcloseinteractortest.cpp
            $<$<TARGET_EXISTS:Qt::lrelease>:applicationtranslatortest.cpp>
            changedetectortest.cpp
            keymapcontextmenuprovidertest.cpp
            keymapcontrollertest.cpp
            keymaptreemodeltest.cpp
            keymappagetest.cpp
            keymappagewidgettest.cpp
            pendingkeymapstatetest.cpp
            loggertest.cpp
            mainwindowgeometryandstatetest.cpp
            mainwindowtest.cpp
            menucontainertest.cpp
            multicolumnsortfilterproxymodeltest.cpp
            mockaboutdialog.cpp
            mockapplicationcloseview.cpp
            mockkeymappagewidget.cpp
            mockmainwindowview.cpp
            mocksettingsdialog.cpp
            nulllogger.cpp
            qtsettingstest.cpp
            searchfilterresultdelegatetest.cpp
            searchlineedittest.cpp
            settingsdialoggeometryandstatetest.cpp
            settingspagefilterproxymodeltest.cpp
            settingspagegrouptreemodeltest.cpp
            settingsdialogcontrollertest.cpp
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
          Aide::AideCore
          Aide::AideGui
          Aide::AideLogger
          Aide::AideUtils
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

if(MSVC AND BUILD_SHARED_LIBS)
  target_compile_definitions(ut.aide PRIVATE BUILD_SHARED_LIBS_ON_MSVC)
endif()

add_executable(ut.aide.application)

target_sources(ut.aide.application PRIVATE applicationtest.cpp mocksettings.cpp)

target_link_libraries(
  ut.aide.application
  PRIVATE aide_catch_main_application
          Qt::Core
          Qt::Widgets
          Qt::Test
          aide_coverage
          aide_options
          aide_warnings
          Aide::AideBuildInformation
          Aide::AideApplication
          Aide::AideCore
          Aide::AideGui
          Aide::AideLogger
          Aide::AideUtils
          Aide::AideWidgets
)

if(NOT PROJECT_IS_TOP_LEVEL)
  target_include_directories(
    ut.aide.application PRIVATE ${CMAKE_SOURCE_DIR}/aide/src/include
                                ${CMAKE_SOURCE_DIR}/aide/src/include/aide
  )
endif()

if(APPLE)
  add_custom_command(
    TARGET ut.aide.application
    POST_BUILD
    COMMAND ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${Qt5_DIR}/../../"
            $<TARGET_FILE:ut.aide.application>
  )
endif()

if(MSVC AND BUILD_SHARED_LIBS)
  target_compile_definitions(
    ut.aide.application PRIVATE BUILD_SHARED_LIBS_ON_MSVC
  )
endif()

enable_testing()

function(aide_discover_unit_tests target)
  if(UNIT_TESTS_RUN_ON_CI)
    catch_discover_tests(
      ${target}
      EXTRA_ARGS
      --reporter
      compact::out=-
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
      ${target} REPORTER console WORKING_DIRECTORY
      ${CMAKE_CURRENT_LIST_DIR}/../../
    )
  endif()
endfunction()

aide_discover_unit_tests(ut.aide)
aide_discover_unit_tests(ut.aide.application)

set_source_files_properties(
  loggertest.cpp
  PROPERTIES COMPILE_DEFINITIONS
             TEST_LOG_FILE_LOCATION="${CMAKE_CURRENT_BINARY_DIR}"
)
