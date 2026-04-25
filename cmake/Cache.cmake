# Enable cache if available
function(aide_enable_cache)
  set(AIDE_CACHE_OPTION "ccache" CACHE STRING "Compiler cache to be used")
  set(AIDE_CACHE_OPTION_VALUES "ccache" "sccache")
  set_property(
    CACHE AIDE_CACHE_OPTION
    PROPERTY STRINGS ${AIDE_CACHE_OPTION_VALUES}
  )
  list(
    FIND AIDE_CACHE_OPTION_VALUES
    ${AIDE_CACHE_OPTION}
    AIDE_CACHE_OPTION_INDEX
  )

  if(${AIDE_CACHE_OPTION_INDEX} EQUAL -1)
    message(
      STATUS
      "Using custom compiler cache system: '${AIDE_CACHE_OPTION}', explicitly supported entries are ${AIDE_CACHE_OPTION_VALUES}"
    )
  endif()

  find_program(CACHE_BINARY NAMES ${AIDE_CACHE_OPTION_VALUES})
  if(CACHE_BINARY)
    message(STATUS "${CACHE_BINARY} found and enabled")
    set(
      CMAKE_CXX_COMPILER_LAUNCHER
      ${CACHE_BINARY}
      CACHE FILEPATH
      "CXX compiler cache used"
    )
    set(
      CMAKE_C_COMPILER_LAUNCHER
      ${CACHE_BINARY}
      CACHE FILEPATH
      "C compiler cache used"
    )
  else()
    message(
      WARNING
      "${AIDE_CACHE_OPTION} is enabled but was not found. Not using it"
    )
  endif()
endfunction()
