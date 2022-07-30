include(GNUInstallDirs)

install(
  TARGETS AideQt${QT_VERSION_MAJOR}Application
          AideBuildInformation
          AideCore
          AideGui
          AideLogger
          AideQt${QT_VERSION_MAJOR}Widgets
          aide_project_warnings
          aide_project_coverage
          aide_project_sanitizers
  EXPORT AideTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT AideTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AideQt${QT_VERSION_MAJOR}
  FILE AideQt${QT_VERSION_MAJOR}Targets.cmake
  NAMESPACE Aide::
)

export(
  EXPORT AideTargets
  FILE "${CMAKE_BINARY_DIR}/AideQt${QT_VERSION_MAJOR}Targets.cmake"
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
  "${CMAKE_BINARY_DIR}/AideQt${QT_VERSION_MAJOR}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/AideQt${QT_VERSION_MAJOR}"
  NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES ${CMAKE_BINARY_DIR}/AideQt${QT_VERSION_MAJOR}Config.cmake
              ${CMAKE_BINARY_DIR}/AideConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AideQt${QT_VERSION_MAJOR}
)

include(CPack)
