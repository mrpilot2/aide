cmake_policy(SET CMP0007 NEW)

function(unescape_args)
  foreach(arg ${ARGV})
    if(NOT ${arg})
      continue()
    endif()

    list(REMOVE_DUPLICATES ${arg})
    list(REMOVE_ITEM ${arg} "")

    string(REPLACE ";" " " ${arg} "${${arg}}")
    set(${arg}
        "${${arg}}"
        PARENT_SCOPE
    )
  endforeach()
endfunction()

unescape_args(GIT_HASH APP_COMPILE_FLAGS)

message(
  "Writing buildinformation file from ${CMAKE_CURRENT_LIST_DIR}/buildinformation.hpp.in to ${OUTPUT_PATH}/include/aide/buildinformation.hpp"
)

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/buildinformation.hpp.in
  ${OUTPUT_PATH}/include/aide/buildinformation.hpp
)
