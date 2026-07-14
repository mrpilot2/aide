add_subdirectory(utils)
add_subdirectory(core)
add_subdirectory(gui)
add_subdirectory(logger)

add_library(AideQt${QT_VERSION_MAJOR}Application)
add_library(Aide::AideApplication ALIAS AideQt${QT_VERSION_MAJOR}Application)

target_sources(
  AideQt${QT_VERSION_MAJOR}Application
  PRIVATE application.cpp applicationbuilder.cpp
)

target_sources(
  AideQt${QT_VERSION_MAJOR}Application
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
      FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/aide/application.hpp
)

target_include_directories(
  AideQt${QT_VERSION_MAJOR}Application
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/aide/>
)

target_link_libraries(
  AideQt${QT_VERSION_MAJOR}Application
  PUBLIC spdlog::spdlog Qt::Core Qt::Widgets
  PRIVATE
    Aide::AideCore
    Aide::AideGui
    Aide::AideUtils
    aide_coverage
    aide_options
    aide_warnings
)

target_compile_features(AideQt${QT_VERSION_MAJOR}Application PUBLIC cxx_std_20)

if(WIN32)
  target_compile_definitions(
    AideCore
    PUBLIC _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
  )
endif()

if(MINIMAL_RUNTIME AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  target_compile_options(
    AideQt${QT_VERSION_MAJOR}Application
    PUBLIC -fsanitize-minimal-runtime -fsanitize=undefined
  )
  target_link_options(
    AideQt${QT_VERSION_MAJOR}Application
    PUBLIC -fsanitize-minimal-runtime -fsanitize=undefined
  )
endif()

add_subdirectory(buildinformation)
