# automatically enable catch2 to generate ctest targets
if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)
target_compile_features(catch_main PUBLIC cxx_std_11)

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
    loggertest.cpp
    searchlineedittest.cpp
    $<$<TARGET_EXISTS:Qt5::lrelease>:${CMAKE_CURRENT_BINARY_DIR}/ut_translations.qrc
    ${QM_FILES}>
    res/ut_icons.qrc
)

if(NOT ${TEST_AGAINST_INSTALLED})
  target_sources(
    ut.aide PRIVATE actionregistrytest.cpp applicationtranslatortest.cpp
                    qtsettingstest.cpp
  )
endif()

target_link_libraries(
  ut.aide
  PRIVATE catch_main
          Qt5::Core
          Qt5::Widgets
          Qt5::Test
          project_sanitizers
          project_coverage
          project_warnings
          Aide::AideBuildInformation
          Aide::AideCore
)

if(NOT ${TEST_AGAINST_INSTALLED})
  target_include_directories(
    ut.aide PRIVATE ${CMAKE_SOURCE_DIR}/aide/src/include
                    ${CMAKE_SOURCE_DIR}/aide/src/include/aide
  )
endif()

enable_testing()
catch_discover_tests(ut.aide EXTRA_ARGS --reporter=console)
