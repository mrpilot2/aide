macro(standard_project_settings)
  # Set a default build type if none was specified
  get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
  if(NOT ${IS_MULTI_CONFIG}
     AND NOT CMAKE_BUILD_TYPE
     AND NOT CMAKE_CONFIGURATION_TYPES
  )
    message(
      STATUS "Setting build type to 'RelWithDebInfo' as none was specified."
    )
    set(CMAKE_BUILD_TYPE
        RelWithDebInfo
        CACHE STRING "Choose the type of build." FORCE
    )
    # Set the possible values of build type for cmake-gui, ccmake
    set_property(
      CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel"
                                      "RelWithDebInfo"
    )
  endif()

  find_program(CCACHE ccache)
  if(CCACHE)
    message("using ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
  else()
    message("ccache not found cannot use")
  endif()

  # Generate compile_commands.json to make it easier to work with clang based
  # tools
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  if (WIN32)
    SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  endif()

endmacro()
