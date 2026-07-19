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
    notificationlauncherdialogtest.cpp
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
)

enable_testing()
catch_discover_tests(
  ut.aide.demo REPORTER console WORKING_DIRECTORY
  ${CMAKE_CURRENT_LIST_DIR}/../../
)
