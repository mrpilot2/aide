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
    "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH
      SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
    TLS_VERIFY ON
  )
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

conan_add_remote(
  NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan
)

conan_cmake_configure(
  REQUIRES spdlog/1.8.5 fmt/7.1.3 GENERATORS cmake_find_package
)

conan_cmake_autodetect(settings)

conan_cmake_install(
  PATH_OR_REFERENCE
  .
  BUILD
  missing
  REMOTE
  conan-center
  SETTINGS
  ${settings}
)

find_package(spdlog)
find_package(fmt)

if(TARGET spdlog::spdlog)
  set_target_properties(spdlog::spdlog PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

if(TARGET fmt::fmt)
  set_target_properties(fmt::fmt PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

if(TARGET Qt5::Core)
  set_target_properties(
    Qt5::Core Qt5::Widgets Qt5::moc Qt5::uic Qt5::rcc PROPERTIES IMPORTED_GLOBAL
                                                                 TRUE
  )
  set(Qt5Core_VERSION_MAJOR
      ${Qt5Core_VERSION_MAJOR}
      PARENT_SCOPE
  )
  set(Qt5Core_VERSION_MINOR
      ${Qt5Core_VERSION_MINOR}
      PARENT_SCOPE
  )
endif()

find_package(Qt5LinguistTools)

if(TARGET Qt5::lrelease)
  set_target_properties(
    Qt5::lrelease Qt5::lupdate PROPERTIES IMPORTED_GLOBAL TRUE
  )
  set(Qt5_LRELEASE_EXECUTABLE
      ${Qt5_LRELEASE_EXECUTABLE}
      PARENT_SCOPE
  )
  set(Qt5_LUPDATE_EXECUTABLE
      ${Qt5_LUPDATE_EXECUTABLE}
      PARENT_SCOPE
  )
endif()
