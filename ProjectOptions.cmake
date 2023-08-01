include(cmake/SystemLink.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

macro(aide_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND NOT WIN32
  )
    set(SUPPORTS_UBSAN ON)
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND WIN32
  )
    set(SUPPORTS_ASAN OFF)
  else()
    set(SUPPORTS_ASAN ON)
  endif()
endmacro()

macro(aide_setup_options)
  option(aide_ENABLE_HARDENING "Enable hardening" ON)
  option(aide_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    aide_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies" ON
    aide_ENABLE_HARDENING OFF
  )

  aide_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR aide_PACKAGING_MAINTAINER_MODE)
    option(aide_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(aide_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(aide_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(aide_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(aide_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(aide_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(aide_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(aide_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(aide_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(aide_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(aide_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(aide_ENABLE_PCH "Enable precompiled headers" OFF)
    option(aide_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(aide_ENABLE_IPO "Enable IPO/LTO" ON)
    option(aide_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(aide_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(aide_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer"
           ${SUPPORTS_ASAN}
    )
    option(aide_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(aide_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer"
           ${SUPPORTS_UBSAN}
    )
    option(aide_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(aide_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(aide_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(aide_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(aide_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(aide_ENABLE_PCH "Enable precompiled headers" OFF)
    option(aide_ENABLE_CACHE "Enable ccache" ON)
  endif()

  add_feature_info(ENABLE_IPO aide_ENABLE_IPO "Enable IPO/LTO")
  add_feature_info(
    WARNINGS_AS_ERRORS aide_WARNINGS_AS_ERRORS "Treat Warnings As Errors"
  )
  add_feature_info(
    ENABLE_USER_LINKER aide_ENABLE_USER_LINKER "Enable user-selected linker"
  )
  add_feature_info(
    ENABLE_SANITIZER_ADDRESS aide_ENABLE_SANITIZER_ADDRESS
    "Enable address sanitizer"
  )
  add_feature_info(
    ENABLE_SANITIZER_LEAK aide_ENABLE_SANITIZER_LEAK "Enable leak sanitizer"
  )
  add_feature_info(
    ENABLE_SANITIZER_UNDEFINED aide_ENABLE_SANITIZER_UNDEFINED
    "Enable undefined sanitizer"
  )
  add_feature_info(
    ENABLE_SANITIZER_THREAD aide_ENABLE_SANITIZER_THREAD
    "Enable thread sanitizer"
  )
  add_feature_info(
    ENABLE_SANITIZER_MEMORY aide_ENABLE_SANITIZER_MEMORY
    "Enable memory sanitizer"
  )
  add_feature_info(
    ENABLE_UNITY_BUILD aide_ENABLE_UNITY_BUILD "Enable unity builds"
  )
  add_feature_info(ENABLE_CLANG_TIDY aide_ENABLE_CLANG_TIDY "Enable clang-tidy")
  add_feature_info(
    ENABLE_CPPCHECK aide_ENABLE_CPPCHECK "Enable cpp-check analysis"
  )
  add_feature_info(ENABLE_PCH aide_ENABLE_PCH "Enable precompiled headers")
  add_feature_info(ENABLE_CACHE aide_ENABLE_CACHE "Enable ccache")

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      aide_ENABLE_IPO
      aide_WARNINGS_AS_ERRORS
      aide_ENABLE_USER_LINKER
      aide_ENABLE_SANITIZER_ADDRESS
      aide_ENABLE_SANITIZER_LEAK
      aide_ENABLE_SANITIZER_UNDEFINED
      aide_ENABLE_SANITIZER_THREAD
      aide_ENABLE_SANITIZER_MEMORY
      aide_ENABLE_UNITY_BUILD
      aide_ENABLE_CLANG_TIDY
      aide_ENABLE_CPPCHECK
      aide_ENABLE_COVERAGE
      aide_ENABLE_PCH
      aide_ENABLE_CACHE
    )
  endif()

endmacro()

macro(aide_global_options)
  if(aide_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    aide_enable_ipo()
  endif()

  aide_supports_sanitizers()

  if(aide_ENABLE_HARDENING AND aide_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR aide_ENABLE_SANITIZER_UNDEFINED
       OR aide_ENABLE_SANITIZER_ADDRESS
       OR aide_ENABLE_SANITIZER_THREAD
       OR aide_ENABLE_SANITIZER_LEAK
    )
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message(
      "${aide_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${aide_ENABLE_SANITIZER_UNDEFINED}"
    )
    aide_enable_hardening(aide_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(aide_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(aide_warnings INTERFACE)
  add_library(aide_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  aide_set_project_warnings(
    aide_warnings ${aide_WARNINGS_AS_ERRORS} "" "" "" ""
  )

  if(aide_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    configure_linker(aide_options)
  endif()

  include(cmake/Sanitizers.cmake)
  aide_enable_sanitizers(
    aide_options ${aide_ENABLE_SANITIZER_ADDRESS} ${aide_ENABLE_SANITIZER_LEAK}
    ${aide_ENABLE_SANITIZER_UNDEFINED} ${aide_ENABLE_SANITIZER_THREAD}
    ${aide_ENABLE_SANITIZER_MEMORY}
  )

  set_target_properties(
    aide_options PROPERTIES UNITY_BUILD ${aide_ENABLE_UNITY_BUILD}
  )

  if(aide_ENABLE_PCH)
    target_precompile_headers(
      aide_options INTERFACE <vector> <string> <utility>
    )
  endif()

  if(aide_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    aide_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(aide_ENABLE_CLANG_TIDY)
    aide_enable_clang_tidy(aide_options ${aide_WARNINGS_AS_ERRORS})
  endif()

  if(aide_ENABLE_CPPCHECK)
    aide_enable_cppcheck(
      ${aide_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(aide_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    aide_enable_coverage(aide_options)
  endif()

  if(aide_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(aide_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(aide_ENABLE_HARDENING AND NOT aide_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR aide_ENABLE_SANITIZER_UNDEFINED
       OR aide_ENABLE_SANITIZER_ADDRESS
       OR aide_ENABLE_SANITIZER_THREAD
       OR aide_ENABLE_SANITIZER_LEAK
    )
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    aide_enable_hardening(aide_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
