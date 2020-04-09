include(GetGitRevisionDescription)

git_describe(GIT_HASH --tags --always --dirty)

message("BuildInformation - Current git hash is " ${GIT_HASH})

configure_file(
  buildinformation.hpp.in ${CMAKE_BINARY_DIR}/include/aide/buildinformation.hpp
)

set_source_files_properties(
  ${CMAKE_BINARY_DIR}/include/aide/buildinformation.hpp PROPERTIES GENERATED
                                                                   TRUE
)

add_library(AideBuildInformation INTERFACE)
add_library(Aide::AideBuildInformation ALIAS AideBuildInformation)

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
