add_library(AideVcs)
add_library(Aide::AideVcs ALIAS AideVcs)

target_sources(AideVcs PRIVATE vcsmanager.cpp vcsmanager.hpp)

target_link_libraries(
  AideVcs
  PUBLIC Aide::AideCore
  PRIVATE
    $<BUILD_LOCAL_INTERFACE:$<$<TARGET_EXISTS:aide_coverage>:aide_coverage>>
    $<BUILD_LOCAL_INTERFACE:$<$<TARGET_EXISTS:aide_options>:aide_options>>
    $<BUILD_LOCAL_INTERFACE:$<$<TARGET_EXISTS:aide_warnings>:aide_warnings>>
)
