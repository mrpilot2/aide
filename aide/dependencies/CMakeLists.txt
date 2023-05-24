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
    "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake" TLS_VERIFY ON
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

    conan_cmake_install(
      PATH_OR_REFERENCE
      .
      BUILD
      missing
      SETTINGS
      ${settings}
      OUTPUT_QUIET
    )
  endforeach()
else()
  conan_cmake_autodetect(settings)

  conan_cmake_install(
    PATH_OR_REFERENCE
    .
    BUILD
    missing
    SETTINGS
    ${settings}
    OUTPUT_QUIET
  )
endif()

find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

if(TARGET spdlog::spdlog)
  set_target_properties(spdlog::spdlog PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

if(TARGET fmt::fmt)
  set_target_properties(fmt::fmt PROPERTIES IMPORTED_GLOBAL TRUE)
endif()
