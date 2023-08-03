execute_process(
  COMMAND
    ${ABI_CHECKER} -l libAide -v1 ${LATEST_RELEASE_TAG} -old
    ${CMAKE_BINARY_DIR}/abibase/build/aide/abi_compliance_config.xml -v2
    ${GIT_HASH} -new ${CMAKE_BINARY_DIR}/aide/abi_compliance_config.xml
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND_ECHO STDOUT
  RESULT_VARIABLE res_var
)

if(NOT "${res_var}" STREQUAL "0")
  message(STATUS "ABI compliance checker returned with res_var='${res_var}'")

  if(NOT "${res_var}" STREQUAL "1")
    message(
      FATAL_ERROR
        "ABI compliance checker error ${res_var}. Status of Binary and Source compatibility cannot be checked from return-code. Fix ABI compliance checker error and try again."
    )
  else()
    message(
      FATAL_ERROR
      "ABI compliance checker detected incompability. If any commit between ${GIT_HASH} (HEAD) and ${LATEST_RELEASE_TAG} is marked as braking change, this is OK."
    )

  endif()
else()
  message(
    STATUS
      "Generating ABI compliance report finished without any error. Binary and Source compatibility is given."
  )
endif()
