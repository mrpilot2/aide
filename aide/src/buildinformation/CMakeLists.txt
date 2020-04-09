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

add_library(aide_buildinformation INTERFACE)
add_library(Aide::BuildInformation ALIAS aide_buildinformation)

target_sources(
  aide_buildinformation
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/aide/buildinformation.hpp
)

target_include_directories(
  aide_buildinformation INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(aide_buildinformation INTERFACE aide::project_coverage)

target_compile_features(aide_buildinformation INTERFACE cxx_std_14)
