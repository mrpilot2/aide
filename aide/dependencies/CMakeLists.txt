list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan"
  )
  file(
    DOWNLOAD
    "https://raw.githubusercontent.com/conan-io/cmake-conan/7f9fe6df98eb805d78b7e55f27920546dbdd8d49/conan.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH
      SHA256=7496444eb14a0fd8be75c6a158f0b693ccb3bc4404806e12df61d3db4bb070d2
    TLS_VERIFY ON
  )
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

conan_add_remote(NAME conancenter URL https://center.conan.io)

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(${IS_MULTI_CONFIG})
  set(generator cmake_find_package_multi)
else()
  set(generator cmake_find_package)
endif()

conan_cmake_configure(REQUIRES spdlog/1.8.5 fmt/7.1.3 GENERATORS ${generator})

if(${IS_MULTI_CONFIG})
  foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
    conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})

    conan_cmake_install(PATH_OR_REFERENCE . BUILD missing SETTINGS ${settings})
  endforeach()
else()
  conan_cmake_autodetect(settings)

  conan_cmake_install(PATH_OR_REFERENCE . BUILD missing SETTINGS ${settings})
endif()
find_package(spdlog)
find_package(fmt)

if(TARGET spdlog::spdlog)
  set_target_properties(spdlog::spdlog PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

if(TARGET fmt::fmt)
  set_target_properties(fmt::fmt PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

find_package(
  Qt5
  COMPONENTS Core Widgets
  REQUIRED
)

if(TARGET Qt5::Core)
  set_target_properties(
    Qt5::Core Qt5::Widgets Qt5::moc Qt5::uic Qt5::rcc PROPERTIES IMPORTED_GLOBAL
                                                                 TRUE
  )
  if(NOT TARGET Qt::Core)
    add_library(Qt::Core ALIAS Qt5::Core)
    add_library(Qt::Widgets ALIAS Qt5::Widgets)
    add_executable(Qt::moc ALIAS Qt5::moc)
    add_executable(Qt::uic ALIAS Qt5::uic)
    add_executable(Qt::rcc ALIAS Qt5::rcc)
  else()
    set_target_properties(
      Qt::Core Qt::Widgets Qt::moc Qt::uic Qt::rcc PROPERTIES IMPORTED_GLOBAL
                                                              TRUE
    )
  endif()

  set(Qt5Core_VERSION_MAJOR
      "${Qt5Core_VERSION_MAJOR}"
      CACHE INTERNAL "Qt5 Major version"
  )
  set(Qt5Core_VERSION_MINOR
      "${Qt5Core_VERSION_MINOR}"
      CACHE INTERNAL "Qt5 Minor version"
  )
endif()

find_package(Qt5LinguistTools)

if(TARGET Qt5::lrelease)
  set_target_properties(
    Qt5::lrelease Qt5::lupdate PROPERTIES IMPORTED_GLOBAL TRUE
  )
  if(NOT TARGET Qt::lrelease)
    add_executable(Qt::lrelease ALIAS Qt5::lrelease)
    add_executable(Qt::lupdate ALIAS Qt5::lupdate)
  else()
    set_target_properties(
      Qt::lrelease Qt::lupdate PROPERTIES IMPORTED_GLOBAL TRUE
    )
  endif()

  set(Qt5_LRELEASE_EXECUTABLE
      ${Qt5_LRELEASE_EXECUTABLE}
      PARENT_SCOPE
  )
  set(Qt5_LUPDATE_EXECUTABLE
      ${Qt5_LUPDATE_EXECUTABLE}
      PARENT_SCOPE
  )
endif()
