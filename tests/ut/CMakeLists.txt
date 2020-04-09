# automatically enable catch2 to generate ctest targets
if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)
target_compile_features(catch_main PUBLIC cxx_std_11)

add_executable(ut.aide)

target_sources(ut.aide PRIVATE buildinformationtest.cpp loggertest.cpp)

target_link_libraries(
  ut.aide PRIVATE catch_main project_sanitizers Aide::AideBuildInformation
                  Aide::AideCore
)

enable_testing()
catch_discover_tests(ut.aide EXTRA_ARGS --reporter=junit --out=test_result.xml)
