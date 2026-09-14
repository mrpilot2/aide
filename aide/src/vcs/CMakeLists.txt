set(CMAKE_AUTOMOC ON)

add_library(AideVcs)
add_library(Aide::AideVcs ALIAS AideVcs)

target_sources(
  AideVcs
  PRIVATE
    vcscommandlineredactor.cpp
    vcscommandlineredactor.hpp
    vcsmanager.cpp
    vcsmanager.hpp
    vcsprocessrunner.cpp
    vcsprocessrunner.hpp
    vcsrepository.cpp
    vcsrepository.hpp
)

target_include_directories(
  AideVcs
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
  AideVcs
  PUBLIC Aide::AideCore
  PRIVATE
    $<BUILD_LOCAL_INTERFACE:$<$<TARGET_EXISTS:aide_coverage>:aide_coverage>>
    $<BUILD_LOCAL_INTERFACE:$<$<TARGET_EXISTS:aide_options>:aide_options>>
    $<BUILD_LOCAL_INTERFACE:$<$<TARGET_EXISTS:aide_warnings>:aide_warnings>>
)
