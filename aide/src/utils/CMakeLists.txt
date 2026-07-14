add_library(AideUtils)
add_library(Aide::AideUtils ALIAS AideUtils)

if(WIN32)
  target_sources(AideUtils PRIVATE systemmemory_win.cpp)
elseif(UNIX AND NOT APPLE)
  target_sources(AideUtils PRIVATE systemmemory_lin.cpp)
else()
  target_sources(AideUtils PRIVATE systemmemory_mac.cpp)
endif()

target_sources(
  AideUtils
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include
      FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/utils/systemmemory.hpp
)

target_include_directories(
  AideUtils
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  AideUtils
  PRIVATE aide_coverage aide_options aide_warnings
)

target_compile_features(AideUtils PUBLIC cxx_std_17)
