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

conan_cmake_configure(REQUIRES catch2/2.13.6 GENERATORS ${generator})

if(${IS_MULTI_CONFIG})
  foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
    conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})

    conan_cmake_install(PATH_OR_REFERENCE . BUILD missing SETTINGS ${settings})
  endforeach()
else()
  conan_cmake_autodetect(settings)

  conan_cmake_install(PATH_OR_REFERENCE . BUILD missing SETTINGS ${settings})
endif()
find_package(Catch2)

find_package(
  Qt5
  COMPONENTS Widgets Test
  REQUIRED
)

if(NOT TARGET Qt::Test)
  add_library(Qt::Test ALIAS Qt5::Test)
  add_library(Qt::Widgets ALIAS Qt5::Widgets)
endif()

find_package(Qt5LinguistTools)
