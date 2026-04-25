include(SystemLink)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(FeatureSummary)

macro(aide_supports_sanitizers)
  if(
    (
      CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*"
      OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*"
    )
    AND NOT WIN32
  )
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if(
    (
      CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*"
      OR CMAKE_CXX_COMPILER_ID MATCHES ".*GNU.*"
    )
    AND WIN32
  )
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(aide_setup_options)
  option(AIDE_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  option(AIDE_PACKAGING_MAINTAINER_MODE "Enable packaging maintainer mode" OFF)

  if(PROJECT_IS_TOP_LEVEL)
    option(AIDE_ENABLE_HARDENING "Enable hardening" ON)
  else()
    option(AIDE_ENABLE_HARDENING "Enable hardening" OFF)
  endif()

  cmake_dependent_option(
    AIDE_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    AIDE_ENABLE_HARDENING
    OFF
  )

  aide_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR AIDE_PACKAGING_MAINTAINER_MODE)
    option(AIDE_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(AIDE_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(AIDE_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(AIDE_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(AIDE_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(AIDE_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(AIDE_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(AIDE_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(AIDE_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(AIDE_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(AIDE_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(AIDE_ENABLE_PCH "Enable precompiled headers" OFF)
    option(AIDE_ENABLE_CACHE "Enable ccache" OFF)
    option(aide_ENABLE_ABI_COMPLIANCE_CHECK "Enable ABI compliance check" OFF)
  else()
    option(AIDE_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(AIDE_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(AIDE_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(
      AIDE_ENABLE_SANITIZER_ADDRESS
      "Enable address sanitizer"
      ${SUPPORTS_ASAN}
    )
    option(AIDE_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(
      AIDE_ENABLE_SANITIZER_UNDEFINED
      "Enable undefined sanitizer"
      ${SUPPORTS_UBSAN}
    )
    option(AIDE_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(AIDE_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(AIDE_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(AIDE_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(AIDE_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(AIDE_ENABLE_PCH "Enable precompiled headers" OFF)
    option(AIDE_ENABLE_CACHE "Enable ccache" ON)
    option(aide_ENABLE_ABI_COMPLIANCE_CHECK "Enable ABI compliance check" OFF)
  endif()

  add_feature_info(
    AIDE_ENABLE_COVERAGE
    AIDE_ENABLE_COVERAGE
    "Enable coverage reporting"
  )
  add_feature_info(
    AIDE_ENABLE_HARDENING
    AIDE_ENABLE_HARDENING
    "Enable hardening"
  )
  add_feature_info(
    AIDE_ENABLE_GLOBAL_HARDENING
    AIDE_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
  )

  add_feature_info(AIDE_ENABLE_IPO AIDE_ENABLE_IPO "Enable IPO/LTO")
  add_feature_info(
    AIDE_WARNINGS_AS_ERRORS
    AIDE_WARNINGS_AS_ERRORS
    "Treat Warnings As Errors"
  )
  add_feature_info(
    AIDE_ENABLE_USER_LINKER
    AIDE_ENABLE_USER_LINKER
    "Enable user-selected linker"
  )
  add_feature_info(
    AIDE_ENABLE_SANITIZER_ADDRESS
    AIDE_ENABLE_SANITIZER_ADDRESS
    "Enable address sanitizer"
  )
  add_feature_info(
    AIDE_ENABLE_SANITIZER_LEAK
    AIDE_ENABLE_SANITIZER_LEAK
    "Enable leak sanitizer"
  )
  add_feature_info(
    AIDE_ENABLE_SANITIZER_UNDEFINED
    AIDE_ENABLE_SANITIZER_UNDEFINED
    "Enable undefined sanitizer"
  )
  add_feature_info(
    AIDE_ENABLE_SANITIZER_THREAD
    AIDE_ENABLE_SANITIZER_THREAD
    "Enable thread sanitizer"
  )
  add_feature_info(
    AIDE_ENABLE_SANITIZER_MEMORY
    AIDE_ENABLE_SANITIZER_MEMORY
    "Enable memory sanitizer"
  )
  add_feature_info(
    AIDE_ENABLE_UNITY_BUILD
    AIDE_ENABLE_UNITY_BUILD
    "Enable unity builds"
  )
  add_feature_info(
    AIDE_ENABLE_CLANG_TIDY
    AIDE_ENABLE_CLANG_TIDY
    "Enable clang-tidy"
  )
  add_feature_info(
    AIDE_ENABLE_CPPCHECK
    AIDE_ENABLE_CPPCHECK
    "Enable cpp-check analysis"
  )
  add_feature_info(AIDE_ENABLE_PCH AIDE_ENABLE_PCH "Enable precompiled headers")
  add_feature_info(AIDE_ENABLE_CACHE AIDE_ENABLE_CACHE "Enable ccache")
  add_feature_info(
    aide_ENABLE_ABI_COMPLIANCE_CHECK
    aide_ENABLE_ABI_COMPLIANCE_CHECK
    "Enable ABI compliance check"
  )

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      AIDE_ENABLE_IPO
      AIDE_WARNINGS_AS_ERRORS
      AIDE_ENABLE_USER_LINKER
      AIDE_ENABLE_SANITIZER_ADDRESS
      AIDE_ENABLE_SANITIZER_LEAK
      AIDE_ENABLE_SANITIZER_UNDEFINED
      AIDE_ENABLE_SANITIZER_THREAD
      AIDE_ENABLE_SANITIZER_MEMORY
      AIDE_ENABLE_UNITY_BUILD
      AIDE_ENABLE_CLANG_TIDY
      AIDE_ENABLE_CPPCHECK
      AIDE_ENABLE_COVERAGE
      AIDE_ENABLE_PCH
      AIDE_ENABLE_CACHE
      aide_ENABLE_ABI_COMPLIANCE_CHECK
    )
  endif()
endmacro()

macro(aide_global_options)
  if(AIDE_ENABLE_IPO)
    include(InterproceduralOptimization)
    aide_enable_ipo()
  endif()

  aide_supports_sanitizers()

  if(AIDE_ENABLE_HARDENING AND AIDE_ENABLE_GLOBAL_HARDENING)
    include(Hardening)
    if(
      NOT SUPPORTS_UBSAN
      OR AIDE_ENABLE_SANITIZER_UNDEFINED
      OR AIDE_ENABLE_SANITIZER_ADDRESS
      OR AIDE_ENABLE_SANITIZER_THREAD
      OR AIDE_ENABLE_SANITIZER_LEAK
    )
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message(
      "${AIDE_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${AIDE_ENABLE_SANITIZER_UNDEFINED}"
    )
    aide_enable_hardening(aide_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(aide_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(StandardProjectSettings)
  endif()

  add_library(aide_warnings INTERFACE)
  add_library(aide_options INTERFACE)
  add_library(aide_coverage INTERFACE)

  include(CompilerWarnings)
  aide_set_project_warnings(
      aide_warnings ${AIDE_WARNINGS_AS_ERRORS} "" "" "" ""
  )

  if(AIDE_ENABLE_USER_LINKER)
    include(Linker)
    configure_linker(aide_options)
  endif()

  include(Sanitizers)
  aide_enable_sanitizers(
      aide_options ${AIDE_ENABLE_SANITIZER_ADDRESS} ${AIDE_ENABLE_SANITIZER_LEAK}
      ${AIDE_ENABLE_SANITIZER_UNDEFINED} ${AIDE_ENABLE_SANITIZER_THREAD}
      ${AIDE_ENABLE_SANITIZER_MEMORY}
  )

  set_target_properties(
    aide_options
    PROPERTIES UNITY_BUILD ${AIDE_ENABLE_UNITY_BUILD}
  )

  if(AIDE_ENABLE_PCH)
    target_precompile_headers(
      aide_options
      INTERFACE <vector> <string> <utility>
    )
  endif()

  if(AIDE_ENABLE_CACHE)
    include(Cache)
    aide_enable_cache()
  endif()

  include(StaticAnalyzers)
  if(AIDE_ENABLE_CLANG_TIDY)
    aide_enable_clang_tidy(aide_options ${AIDE_WARNINGS_AS_ERRORS})
  endif()

  if(AIDE_ENABLE_CPPCHECK)
    aide_enable_cppcheck(
          ${AIDE_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(AIDE_ENABLE_COVERAGE)
    include(Tests)
    aide_enable_coverage(aide_coverage)
  endif()

  if(aide_ENABLE_ABI_COMPLIANCE_CHECK)
    include(AbiComplianceCheck)
    aide_enable_compliance_check(aide_options)
  endif()

  if(AIDE_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(aide_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(AIDE_ENABLE_HARDENING AND NOT AIDE_ENABLE_GLOBAL_HARDENING)
    include(Hardening)
    if(
      NOT SUPPORTS_UBSAN
      OR AIDE_ENABLE_SANITIZER_UNDEFINED
      OR AIDE_ENABLE_SANITIZER_ADDRESS
      OR AIDE_ENABLE_SANITIZER_THREAD
      OR AIDE_ENABLE_SANITIZER_LEAK
    )
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    aide_enable_hardening(aide_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()
