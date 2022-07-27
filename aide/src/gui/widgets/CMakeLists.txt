add_library(AideWidgets)
add_library(Aide::AideWidgets ALIAS AideWidgets)

target_sources(
  AideWidgets
  PRIVATE
    aidetableview.cpp
    aidetreeview.cpp
    searchlineedit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/aidetableview.hpp
    ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/aidetreeview.hpp
    ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/searchlineedit.hpp
)

target_include_directories(
  AideWidgets
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}
          ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
         $<INSTALL_INTERFACE:include/aide>
)

target_link_libraries(
  AideWidgets
  PUBLIC Qt::Widgets
  PRIVATE aide_project_sanitizers aide_project_warnings
)

target_compile_features(AideWidgets PUBLIC cxx_std_17)

set_source_files_properties(
  ${CMAKE_CURRENT_BINARY_DIR}/AideWidgets_autogen/mocs_compilation.cpp
  PROPERTIES COMPILE_FLAGS "$<$<CXX_COMPILER_ID:GNU>:-Wno-useless-cast>"
)
