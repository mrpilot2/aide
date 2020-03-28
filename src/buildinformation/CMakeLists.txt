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

add_library(buildinformation INTERFACE)

target_sources(
  buildinformation
  INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/aide/buildinformation.hpp
)

target_include_directories(
  buildinformation INTERFACE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(buildinformation INTERFACE aide::project_coverage)

target_compile_features(buildinformation INTERFACE cxx_std_11)
