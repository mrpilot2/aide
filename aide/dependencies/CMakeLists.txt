find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)

if(TARGET spdlog::spdlog)
  set_target_properties(spdlog::spdlog PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

if(TARGET fmt::fmt)
  set_target_properties(fmt::fmt PROPERTIES IMPORTED_GLOBAL TRUE)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/addlicensedependency.cmake)

set(table_content "")
set(markdown_content "")

add_license_dependency(
  PROJECT
  "abi-compliance-checker"
  PROJECT_URL
  "https://github.com/lvc/abi-compliance-checker"
  VERSION
  "Undefined"
  LICENSE
  "LGPL-2.1"
  LICENSE_URL
  "https://github.com/lvc/abi-compliance-checker/blob/master/LICENSE"
  MARKDOWN_ONLY
)

add_license_dependency(
  PROJECT
  "cmake-abicc"
  PROJECT_URL
  "https://github.com/Oleh-Kravchenko/cmake-abicc"
  VERSION
  "None"
  LICENSE
  "BSD 3-Clause License"
  LICENSE_URL
  "https://github.com/Oleh-Kravchenko/cmake-abicc/blob/master/LICENSE"
  MARKDOWN_ONLY
)

add_license_dependency(
  PROJECT
  "catch2"
  PROJECT_URL
  "https://github.com/catchorg/Catch2/"
  VERSION
  "2.13.6"
  LICENSE
  "Boost Software License 1.0"
  LICENSE_URL
  "https://github.com/catchorg/Catch2/blob/v2.13.6/LICENSE.txt"
)

add_license_dependency(
  PROJECT
  "fmt"
  PROJECT_URL
  "https://github.com/fmtlib/fmt/"
  VERSION
  "7.1.3"
  LICENSE
  "MIT (custom)"
  LICENSE_URL
  "https://github.com/fmtlib/fmt/blob/7.1.3/LICENSE.rst"
)

add_license_dependency(
  PROJECT
  "libyang (parts used for ABI check)"
  PROJECT_URL
  "https://github.com/CESNET/libyang/blob/master/CMakeModules/ABICheck.cmake"
  VERSION
  "None"
  LICENSE
  "BSD 3-Clause License"
  LICENSE_URL
  "https://github.com/CESNET/libyang/blob/master/LICENSE"
  MARKDOWN_ONLY
)

add_license_dependency(
  PROJECT
  "Qt"
  PROJECT_URL
  "https://www.qt.io/"
  VERSION
  "${QT_VERSION}"
  LICENSE
  "(L)GPL"
  LICENSE_URL
  "https://www.qt.io/licensing/"
)

add_license_dependency(
  PROJECT
  "spdlog"
  PROJECT_URL
  "https://github.com/gabime/spdlog/"
  VERSION
  "1.8.5"
  LICENSE
  "MIT"
  LICENSE_URL
  "https://github.com/gabime/spdlog/blob/v1.8.5/LICENSE"
)

string(LENGTH ${markdown_content} markdown_length)
math(EXPR markdown_substring "${markdown_length} - 2")
string(SUBSTRING ${markdown_content} 0 ${markdown_substring} markdown_content)

configure_file(licenses.hml.in licenses.html @ONLY)
configure_file(
  licenses.md.in ${CMAKE_CURRENT_LIST_DIR}/../../3rd-party-licenses.md @ONLY
)
