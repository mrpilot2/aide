# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(
    STATUS
      "Downloading conan.cmake from https://github.com/conan-io/cmake-conan"
  )
  file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
       "${CMAKE_BINARY_DIR}/conan.cmake"
  )
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_add_remote(
  NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan
)

conan_cmake_run(
  REQUIRES
  ${CONAN_EXTRA_REQUIRES}
  spdlog/1.8.2
  fmt/7.1.2
  OPTIONS
  ${CONAN_EXTRA_OPTIONS}
  BASIC_SETUP
  CMAKE_TARGETS # individual targets to link to
  BUILD
  missing
)

if(TARGET CONAN_PKG::spdlog)
  set_target_properties(CONAN_PKG::spdlog PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

if(TARGET CONAN_PKG::fmt)
  set_target_properties(CONAN_PKG::fmt PROPERTIES IMPORTED_GLOBAL TRUE)
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
