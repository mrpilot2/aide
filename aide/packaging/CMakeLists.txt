include(GNUInstallDirs)

install(
  TARGETS AideApplication AideBuildInformation AideCore AideGui AideLogger
          project_warnings
  EXPORT AideTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT AideTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Aide
  FILE AideTargets.cmake
  NAMESPACE Aide::
)

export(
  EXPORT AideTargets
  FILE "${CMAKE_BINARY_DIR}/AideTargets.cmake"
  NAMESPACE Aide::
)

install(
  DIRECTORY ../src/include/aide
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aide/
  COMPONENT Devel
)

install(
  DIRECTORY ${CMAKE_BINARY_DIR}/include/aide
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/aide/
  COMPONENT Devel
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/AideConfigVersion.cmake COMPATIBILITY SameMajorVersion
)

# generate the config file that is includes the exports
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/AideConfig.cmake.in
  "${CMAKE_BINARY_DIR}/AideConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Aide"
  NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES ${CMAKE_BINARY_DIR}/AideConfig.cmake
              ${CMAKE_BINARY_DIR}/AideConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Aide
)
