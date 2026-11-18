# Acceptance tests for demo-specific widgets (e.g. NotificationLauncherDialog,
# #164). Kept separate from tests/ut so that tree stays buildable standalone
# against an installed Aide package, without requiring demo/ to exist.

# CMAKE_MODULE_PATH's Catch2-contrib entry (added by find_package(Catch2) in
# tests/dependencies/CMakeLists.txt) doesn't cross into this sibling
# directory's scope, so re-resolve it here before include(Catch).
find_package(Catch2 REQUIRED)
include(Catch)

find_package(Qt6 6.5 REQUIRED COMPONENTS Test)

add_executable(ut.aide.demo)

# NotificationLauncherDialog (Q_OBJECT) is compiled straight into this
# executable rather than via a moc-enabled library target, so this target
# needs its own moc pass.
set_target_properties(ut.aide.demo PROPERTIES AUTOMOC ON)

target_sources(
  ut.aide.demo
  PRIVATE
    notificationdemodialogtest.cpp
    notificationlauncherdialogtest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/notificationdemodialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/notificationlauncherdialog.cpp
    # aide_catch_main's catch_main.cpp constructs aide::test::MockSettings;
    # every executable linking that object library must supply its
    # definition, same as tests/ut/CMakeLists.txt's own ut.aide target does.
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/ut/mocksettings.cpp
)

target_include_directories(
  ut.aide.demo
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(
  ut.aide.demo
  PRIVATE
    aide_catch_main
    Qt::Core
    Qt::Widgets
    Qt::Test
    aide_coverage
    aide_options
    aide_warnings
    Aide::AideApplication
    Aide::AideCore
    Aide::AideWidgets
)

enable_testing()
catch_discover_tests(
  ut.aide.demo REPORTER console WORKING_DIRECTORY
  ${CMAKE_CURRENT_LIST_DIR}/../../
)

# Demo tests that construct the demo's aide::Application (e.g. the help
# coverage test, #201). aide::Application is itself a QApplication, so these
# live in their own executable on aide_catch_main_application, which creates
# no process-global QApplication (same split as ut.aide / ut.aide.application).
add_executable(ut.aide.demo.application)

set_target_properties(
  ut.aide.demo.application
  PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../src/res
)

target_sources(
  ut.aide.demo.application
  PRIVATE
    demohelptest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/colorschemereactor.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/demosettingspage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/demosetup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/notificationdemodialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/notificationlauncherdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/reportbugpreviewlauncher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/ut/mocksettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../tests/ut/mockurllauncher.cpp
)

target_include_directories(
  ut.aide.demo.application
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(
  ut.aide.demo.application
  PRIVATE
    aide_catch_main_application
    Qt::Core
    Qt::Widgets
    Qt::Test
    aide_coverage
    aide_options
    aide_warnings
    Aide::AideApplication
    Aide::AideCore
    Aide::AideWidgets
)

if(APPLE)
  add_custom_command(
    TARGET ut.aide.demo.application
    POST_BUILD
    COMMAND
      ${CMAKE_INSTALL_NAME_TOOL} -add_rpath "${Qt6_DIR}/../../"
      $<TARGET_FILE:ut.aide.demo.application>
  )
endif()

catch_discover_tests(
  ut.aide.demo.application REPORTER console WORKING_DIRECTORY
  ${CMAKE_CURRENT_LIST_DIR}/../../
)
