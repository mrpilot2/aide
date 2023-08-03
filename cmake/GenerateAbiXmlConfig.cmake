cmake_policy(SET CMP0007 NEW)

function(unescape_args)
  foreach(arg ${ARGV})
    if(NOT ${arg})
      continue()
    endif()

    list(REMOVE_DUPLICATES ${arg})
    list(REMOVE_ITEM ${arg} "")

    string(REPLACE ";" "\n" ${arg} "${${arg}}")
    set(${arg}
        "${${arg}}"
        PARENT_SCOPE
    )
  endforeach()
endfunction()

unescape_args(
  ABI_VERSION ABI_HEADERS ABI_LIBS ABI_INCLUDE_PATHS ABI_GCC_OPTIONS
)

# unscape DEFINES
list(REMOVE_DUPLICATES ABI_GCC_DEFINES)
foreach(define ${ABI_GCC_DEFINES})
  message(STATUS "Checking ${define}")
  if(define MATCHES "([_a-zA-Z][_a-zA-Z0-9]*)=(.+)")
    list(APPEND defines "-D${CMAKE_MATCH_1}=${CMAKE_MATCH_2}")
  else()
    list(APPEND defines "-D${define}")
  endif()
endforeach()

string(REPLACE ";" "\n" ABI_GCC_DEFINES "${defines}")

list(APPEND ABI_GCC_OPTIONS ${ABI_GCC_DEFINES})
string(REPLACE ";" "\n" ABI_GCC_OPTIONS "${ABI_GCC_OPTIONS}")

set(ABI_GCC_DEFINES "")

if(ABI_SEARCH_LIBS)
  foreach(lib ${ABI_SEARCH_LIBS})
    get_filename_component(dir "${lib}" DIRECTORY)
    list(APPEND search_libs "${dir}")
  endforeach()

  list(REMOVE_DUPLICATES search_libs)
  list(REPLACE ";" "\n" ABI_SEARCH_LIBS "${search_libs}")
endif()

message("Writing file to ${CMAKE_BINARY_DIR}/abi_compliance_check.xml")
# generate XML for abi-compliance-checker
configure_file(
  "${CMAKE_ABI_DIR}/abi_compliance_config.xml.in"
  "${CMAKE_BINARY_DIR}/abi_compliance_config.xml" @ONLY
)
