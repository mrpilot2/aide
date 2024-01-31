include(
  ${CMAKE_CURRENT_LIST_DIR}/../../../cmake/GetGitRevisionDescription.cmake
)

git_describe(GIT_HASH --tags --always --dirty --match "v*")

message("BuildInformation - Current git hash is " ${GIT_HASH})

unset(APP_COMPILE_FLAGS)

set(libs Aide::AideApplication Aide::AideWidgets Aide::AideCore Aide::AideGui)

foreach(lib ${libs})
  list(APPEND APP_COMPILE_FLAGS "$<TARGET_PROPERTY:${lib},COMPILE_OPTIONS>")

  list(APPEND APP_COMPILE_FLAGS "$<TARGET_PROPERTY:${lib},COMPILE_DEFINITIONS>")
endforeach()

list(REMOVE_DUPLICATES APP_COMPILE_FLAGS)
list(REMOVE_ITEM APP_COMPILE_FLAGS "")
string(REPLACE ";" "$<SEMICOLON>" APP_COMPILE_FLAGS "${APP_COMPILE_FLAGS}")

# two step process so that generator expressions are evaluated
add_custom_command(
  OUTPUT "${CMAKE_BINARY_DIR}/include/aide/buildinformation.hpp"
  COMMAND
    ${CMAKE_COMMAND} ARGS -DOUTPUT_PATH=${CMAKE_BINARY_DIR}
    -DCMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}
    -DPROJECT_VERSION="${PROJECT_VERSION}"
    -DPROJECT_VERSION_MAJOR="${PROJECT_VERSION_MAJOR}"
    -DPROJECT_VERSION_MINOR="${PROJECT_VERSION_MINOR}"
    -DPROJECT_VERSION_PATCH="${PROJECT_VERSION_PATCH}"
    -DCMAKE_CXX_COMPILER="${CMAKE_CXX_COMPILER}"
    -DCMAKE_CXX_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
    -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" -DGIT_HASH="${GIT_HASH}"
    -DAPP_COMPILE_FLAGS="${APP_COMPILE_FLAGS}" -P
    "${CMAKE_CURRENT_LIST_DIR}/GenerateBuildInformation.cmake"
  COMMENT "Generate Build information"
)

add_custom_target(
  aide_generate_build_information
  DEPENDS "${CMAKE_BINARY_DIR}/include/aide/buildinformation.hpp"
)

set_source_files_properties(
  ${CMAKE_BINARY_DIR}/include/aide/buildinformation.hpp PROPERTIES GENERATED
                                                                   TRUE
)

add_library(AideBuildInformation INTERFACE)
add_library(Aide::AideBuildInformation ALIAS AideBuildInformation)

add_dependencies(AideBuildInformation aide_generate_build_information)

target_sources(
  AideBuildInformation
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/aide/buildinformation.hpp>
    $<INSTALL_INTERFACE:include/aide/aide/buildinformation.hpp>
)

target_include_directories(
  AideBuildInformation INTERFACE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/>
                                 $<INSTALL_INTERFACE:include/aide>
)

target_link_libraries(AideBuildInformation)

target_compile_features(AideBuildInformation INTERFACE cxx_std_14)
