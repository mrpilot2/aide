set(CURRENT_SCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR})

function(aide_enable_compliance_check project_name)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES
                                             ".*Clang"
  )
    target_compile_options(${project_name} INTERFACE -g -Og)
    target_link_libraries(${project_name} INTERFACE -Og)
  endif()
endfunction()

function(aide_perform_abi_compliance_check)
  # find abi-dumper
  find_program(ABI_DUMPER abi-dumper)
  find_package_handle_standard_args(abi-dumper DEFAULT_MSG ABI_DUMPER)
  if(NOT ABI_DUMPER)
    message(FATAL_ERROR "Program abi-dumper not found!")
  endif()

  # find abi-checker
  find_program(ABI_CHECKER abi-compliance-checker)
  find_package_handle_standard_args(
    abi-compliance-checker DEFAULT_MSG ABI_CHECKER
  )
  if(NOT ABI_CHECKER)
    message(FATAL_ERROR "Program abi-compliance-checker not found!")
  endif()

  unset(ABI_LIBS)
  unset(ABI_HEADERS)
  unset(ABI_INCLUDES)
  unset(ABI_GCC_OPTIONS)
  unset(ABI_GCC_DEFINES)

  foreach(lib ${ARGV})
    get_target_property(headers ${lib} INCLUDE_DIRECTORIES)
    list(APPEND ABI_HEADERS "${headers}")
    list(APPEND ABI_INCLUDES
         "$<TARGET_PROPERTY:${lib},INTERFACE_INCLUDE_DIRECTORIES>"
    )
    list(APPEND ABI_INCLUDES
         "$<TARGET_PROPERTY:${lib},INTERFACE_SYSTEM_INCLUDE_DIRECTORIES>"
    )
    list(APPEND ABI_LIBS "$<TARGET_LINKER_FILE:${lib}>")

    get_target_property(gcc_options ${lib} COMPILE_FLAGS)
    message(STATUS "options ${gcc_options}")
    list(APPEND ABI_GCC_OPTIONS "$<TARGET_PROPERTY:${lib},COMPILE_OPTIONS>")

    list(APPEND ABI_GCC_DEFINES "$<TARGET_PROPERTY:${lib},COMPILE_DEFINITIONS>")
  endforeach()

  list(REMOVE_DUPLICATES ABI_HEADERS)
  string(REPLACE ";" "$<SEMICOLON>" ABI_HEADERS "${ABI_HEADERS}")

  list(REMOVE_DUPLICATES ABI_INCLUDES)
  string(REPLACE ";" "$<SEMICOLON>" ABI_INCLUDES "${ABI_INCLUDES}")

  list(REMOVE_DUPLICATES ABI_LIBS)
  string(REPLACE ";" "$<SEMICOLON>" ABI_LIBS "${ABI_LIBS}")

  list(REMOVE_DUPLICATES ABI_GCC_OPTIONS)
  list(REMOVE_ITEM ABI_GCC_OPTIONS "")
  string(REPLACE ";" "$<SEMICOLON>" ABI_GCC_OPTIONS "${ABI_GCC_OPTIONS}")

  list(REMOVE_DUPLICATES ABI_GCC_DEFINES)
  string(REPLACE ";" "$<SEMICOLON>" ABI_GCC_DEFINES "${ABI_GCC_DEFINES}")

  include(${CURRENT_SCRIPT_DIR}/GetGitRevisionDescription.cmake)
  get_git_head_revision(REFSPEC GIT_HASH)

  add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/abi_compliance_config.xml"
    COMMAND
      ${CMAKE_COMMAND} ARGS -DABI_VERSION="${GIT_HASH}"
      -DABI_HEADERS="${ABI_HEADERS}" -DABI_LIBS="${ABI_LIBS}"
      -DABI_INCLUDE_PATHS="${ABI_INCLUDES}"
      -DABI_GCC_OPTIONS="${ABI_GCC_OPTIONS}"
      -DABI_GCC_DEFINES="${ABI_GCC_DEFINES}"
      -DABI_SEARCH_LIBS="${ABI_SEARCH_LIBS}"
      -DCMAKE_ABI_DIR="${CURRENT_SCRIPT_DIR}" -P
      "${CURRENT_SCRIPT_DIR}/GenerateAbiXmlConfig.cmake"
    COMMENT "Generate ABI xml config"
  )

  add_custom_target(
    generate_abi_compliance_xml
    DEPENDS "${CMAKE_BINARY_DIR}/abi_compliance_config.xml"
  )

  git_describe(LATEST_RELEASE_TAG --tags)

  string(FIND ${LATEST_RELEASE_TAG} "-" pos)
  string(SUBSTRING ${LATEST_RELEASE_TAG} 0 ${pos} LATEST_RELEASE_TAG)

  execute_process(
    COMMAND git merge-base --is-ancestor 4d2d9b265021cc4fa3f6bcba20f4e4d7f5c40957 ${LATEST_RELEASE_TAG}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND_ECHO STDOUT
    RESULT_VARIABLE res_var
  )

  if(NOT "${res_var}" STREQUAL "0")
    set(LATEST_RELEASE_TAG 4d2d9b265021cc4fa3f6bcba20f4e4d7f5c40957)
  endif()

  message(
    STATUS
      "Abi Compliance Checker - compare against latest release ${LATEST_RELEASE_TAG}"
  )

  # get URL for fetching origin
  execute_process(
    COMMAND git remote get-url origin
    OUTPUT_VARIABLE ORIGIN_URL
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  # generate script for generating the base ABI dump
  file(
    GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/abibase.sh
    CONTENT
      "#!/bin/sh
if [ ! -d abibase ]; then mkdir abibase; fi
cd abibase
if [ ! -f build/aide/abi_compliance_config.xml ]; then
if [ -d .git ] && [ \"${LATEST_RELEASE_TAG}\" != \"`git log --pretty=oneline | cut -d' ' -f1`\" ]; then rm -rf .* 2> /dev/null; fi
    if [ ! -d .git ]; then
        git init --initial-branch=develop
        git remote add origin ${ORIGIN_URL}
        git pull origin develop --tags
        git checkout ${LATEST_RELEASE_TAG}
    fi
    if [ ! -d build ]; then mkdir build; fi
    cd build
    cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug -Daide_PACKAGING_MAINTAINER_MODE=ON -Daide_ENABLE_ABI_COMPLIANCE_CHECK=ON -DCMAKE_CXX_COMPILER=/usr/bin/g++
    cmake --build .
    cmake --build . --target generate_abi_compliance_xml
fi
"
  )

  # abi-check target - check ABI compatibility of current version and the base
  # hash version
  add_custom_target(
    generate_abi_compliance_report
    COMMAND bash ./abibase.sh
    COMMAND
      cmake -DABI_CHECKER="${ABI_CHECKER}"
      -DCMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}" -DGIT_HASH="${GIT_HASH}"
      -DLATEST_RELEASE_TAG="${LATEST_RELEASE_TAG}" -P
      "${CURRENT_SCRIPT_DIR}/GenerateAbiComplianceReport.cmake"
    DEPENDS ${ARGV} generate_abi_compliance_xml
    BYPRODUCTS
      ${CMAKE_BINARY_DIR}/compat_reports/libAide_${LATEST_RELEASE_TAG}_to_${GIT_HASH}/compat_report.html
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT
      "Checking ABI compatibility of version ${LATEST_RELEASE_TAG} and revision ${GIT_HASH}"
  )

endfunction()
