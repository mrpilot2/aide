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
      STATUS
        "ABI compliance checker detected incompatibility. If any commit between ${GIT_HASH} (HEAD) and ${LATEST_RELEASE_TAG} is marked as braking change, this is OK."
    )

    execute_process(
      "git rev-list --ancestry-path ${LATEST_RELEASE_TAG}..${GIT_HASH}"
      OUTPUT_VARIABLE GIT_HASHES_TO_BE_CHECKED
    )

    set(found_breaking_change FALSE)
    foreach(git_hash ${GIT_HASHES_TO_BE_CHECKED)
      message(
        STATUS "Checking if commit ${git_hash} is marked as breaking changed."
      )

      execute_process("git show -s ${git_hash}" OUTPUT_VARIABLE COMMIT_MESSAGE)

      if(${COMMIT_MESSAGE} MATCHES ".*BREAKING CHANGE:.*")
        message(
          STATUS
            "Commit ${git_hash} is marked as breaking change. Major version will be increased in next release. All OK"
        )
        set(found_breaking_change TRUE)
      endif()
    endforeach()

    if(NOT ${found_breaking_change})
      message(
        FATAL_ERROR
          "ABI compliance checker detected incompatibility but no commit is marked as breaking change. Find the breaking change and mark the commit as such to make this check pass."
      )
    endif()

  endif()
else()
  message(
    STATUS
      "Generating ABI compliance report finished without any error. Binary and Source compatibility is given."
  )
endif()
