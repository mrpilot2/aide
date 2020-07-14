add_library(AideWidgets OBJECT)
add_library(Aide::Widgets ALIAS AideWidgets)

target_sources(
  AideWidgets
  PRIVATE
    searchlineedit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/searchlineedit.hpp
)

target_include_directories(
  AideWidgets PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets
                      ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/
)

target_link_libraries(
  AideWidgets
  PUBLIC Qt5::Widgets
  PRIVATE project_coverage project_sanitizers project_warnings
)

target_compile_features(AideWidgets PUBLIC cxx_std_17)

set_source_files_properties(
  ${CMAKE_CURRENT_BINARY_DIR}/AideWidgets_autogen/mocs_compilation.cpp
  PROPERTIES COMPILE_FLAGS "$<$<CXX_COMPILER_ID:GNU>:-Wno-useless-cast>"
)
