add_library(AideUtils OBJECT)
add_library(Aide::AideUtils ALIAS AideUtils)

set_target_properties(AideUtils PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(WIN32)
  target_sources(AideUtils PRIVATE systemmemory_win.cpp)
elseif(UNIX AND NOT APPLE)
  target_sources(AideUtils PRIVATE systemmemory_lin.cpp)
else()
  target_sources(AideUtils PRIVATE systemmemory_mac.cpp)
endif()

target_include_directories(
  AideUtils
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
  PUBLIC $<INSTALL_INTERFACE:include/aide/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(AideUtils PRIVATE aide_options aide_warnings)

target_compile_features(AideUtils PUBLIC cxx_std_17)
