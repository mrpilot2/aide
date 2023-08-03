execute_process(
  COMMAND
    ${ABI_CHECKER} -l libAide -v1 ${LATEST_RELEASE_TAG} -old
    ${CMAKE_BINARY_DIR}/abibase/build/aide/abi_compliance_config.xml -v2
    ${GIT_HASH} -new ${CMAKE_BINARY_DIR}/aide/abi_compliance_config.xml
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND_ECHO STDOUT
  RESULT_VARIABLE res_var
)

if(NOT "${res_var}" STREQUAL "0")
  # do something here about the failed "process" call...
  message(STATUS "compliance checker returned with res_var='${res_var}'")

  if(NOT "{res_var}" STREQUAL "1")
    message(
      STATUS
        "Any compliance checker error occured, but binary compability is OK"
    )
  else()
    message(FATAL_ERROR "Binary incompability detectd")
  endif()

endif()
