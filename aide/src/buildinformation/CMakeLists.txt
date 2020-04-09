include(GetGitRevisionDescription)

git_describe(GIT_HASH --tags --always --dirty)

message("BuildInformation - Current git hash is " ${GIT_HASH})

configure_file(
  buildinformation.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/aide/buildinformation.hpp
)

set_source_files_properties(
  ${CMAKE_CURRENT_BINARY_DIR}/aide/buildinformation.hpp PROPERTIES GENERATED
                                                                   TRUE
)

add_library(AideBuildInformation INTERFACE)
add_library(Aide::AideBuildInformation ALIAS AideBuildInformation)

target_sources(
  AideBuildInformation
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/aide/buildinformation.hpp
)

target_include_directories(
  AideBuildInformation INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(AideBuildInformation INTERFACE aide::project_coverage)

target_compile_features(AideBuildInformation INTERFACE cxx_std_14)
