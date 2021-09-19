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
    "https://raw.githubusercontent.com/conan-io/cmake-conan/develop/conan.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake"
    EXPECTED_HASH
      SHA256=ddf0fafacf48b5c4912ecce5701c252532437c40277734cad5f4a8084470adbc
    TLS_VERIFY ON
  )
endif()

include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

conan_add_remote(
  NAME bincrafters URL https://api.bintray.com/conan/bincrafters/public-conan
)

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
  endforeach()
else()
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
endif()
find_package(Catch2)

find_package(
  Qt5
  COMPONENTS Widgets Test
  REQUIRED
)

find_package(Qt5LinguistTools)
