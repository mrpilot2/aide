macro(aide_configure_linker project_name)
  include(CheckCXXCompilerFlag)

  set(AIDE_USER_LINKER_OPTION "lld" CACHE STRING "Linker to be used")
  set(AIDE_USER_LINKER_OPTION_VALUES "lld" "gold" "bfd" "mold")
  set_property(
    CACHE AIDE_USER_LINKER_OPTION
    PROPERTY STRINGS ${AIDE_USER_LINKER_OPTION_VALUES}
  )
  list(
    FIND AIDE_USER_LINKER_OPTION_VALUES
    ${AIDE_USER_LINKER_OPTION}
    AIDE_USER_LINKER_OPTION_INDEX
  )

  if(${AIDE_USER_LINKER_OPTION_INDEX} EQUAL -1)
    message(
      STATUS
      "Using custom linker: '${AIDE_USER_LINKER_OPTION}', explicitly supported entries are ${AIDE_USER_LINKER_OPTION_VALUES}"
    )
  endif()

  if(NOT ENABLE_USER_LINKER)
    return()
  endif()

  set(LINKER_FLAG "-fuse-ld=${AIDE_USER_LINKER_OPTION}")

  check_cxx_compiler_flag(${LINKER_FLAG} CXX_SUPPORTS_USER_LINKER)
  if(CXX_SUPPORTS_USER_LINKER)
    target_compile_options(${project_name} INTERFACE ${LINKER_FLAG})
  endif()
endmacro()
