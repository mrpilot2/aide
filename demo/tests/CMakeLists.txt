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

target_sources(
  ut.aide.demo
  PRIVATE notificationdemodialogtest.cpp notificationlauncherdialogtest.cpp
)

target_link_libraries(
  ut.aide.demo
  PRIVATE
    aide_catch_main
    aide_test_mocks
    aide_demo_lib
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

target_sources(ut.aide.demo.application PRIVATE demohelptest.cpp)

target_link_libraries(
  ut.aide.demo.application
  PRIVATE
    aide_catch_main_application
    aide_test_mocks
    aide_demo_lib
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
