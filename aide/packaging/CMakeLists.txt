include(GNUInstallDirs)

if(NOT TARGET aide_coverage)
  add_library(aide_coverage INTERFACE)
endif()

if(NOT TARGET aide_warnings)
  add_library(aide_warnings INTERFACE)
endif()

if(NOT TARGET aide_options)
  add_library(aide_options INTERFACE)
endif()

install(
  TARGETS AideQt${QT_VERSION_MAJOR}Application
          AideBuildInformation
          AideCore
          AideGui
          AideLogger
          AideUtils
          AideQt${QT_VERSION_MAJOR}Widgets
          aide_coverage
          aide_warnings
          aide_options
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

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/../dependencies/licenses.html
        DESTINATION ${CMAKE_INSTALL_DOCDIR}/
)
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_BINARY_DIR}/AideQt${QT_VERSION_MAJOR}ConfigVersion.cmake
  COMPATIBILITY SameMajorVersion
)

if(NOT EXISTS
   ${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/aideqt${QT_VERSION_MAJOR}-extra.cmake
)
  file(
    WRITE
    ${CMAKE_FIND_PACKAGE_REDIRECTS_DIR}/aideqt${QT_VERSION_MAJOR}-extra.cmake
    "
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(PACKAGE_VERSION_COMPATIBLE TRUE)
set(PACKAGE_VERSION_EXACT TRUE)
"
  )
endif()

# generate the config file that is includes the exports
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/AideConfig.cmake.in
  "${CMAKE_BINARY_DIR}/AideQt${QT_VERSION_MAJOR}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/AideQt${QT_VERSION_MAJOR}"
  NO_SET_AND_CHECK_MACRO NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES ${CMAKE_BINARY_DIR}/AideQt${QT_VERSION_MAJOR}Config.cmake
              ${CMAKE_BINARY_DIR}/AideQt${QT_VERSION_MAJOR}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/AideQt${QT_VERSION_MAJOR}
)

include(CPack)
