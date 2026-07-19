add_library(AideQt${QT_VERSION_MAJOR}Widgets)
add_library(Aide::AideWidgets ALIAS AideQt${QT_VERSION_MAJOR}Widgets)

include(GenerateExportHeader)
generate_export_header(
  AideQt${QT_VERSION_MAJOR}Widgets
  BASE_NAME AideWidgets
  EXPORT_FILE_NAME
    ${CMAKE_CURRENT_BINARY_DIR}/generated_include/aide/gui/widgets/aidewidgets_export.h
)

target_sources(
  AideQt${QT_VERSION_MAJOR}Widgets
  PRIVATE
    aidedialog.cpp
    aidetableview.cpp
    aidetreeview.cpp
    banner.cpp
    gotittooltip.cpp
    multicolumnsortfilterproxymodel.cpp
    notificationballoon.cpp
    searchfilterresultdelegate.cpp
    searchfilterwidget.cpp
    searchlineedit.cpp
)

target_sources(
  AideQt${QT_VERSION_MAJOR}Widgets
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../../include
      FILES
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/aidedialog.hpp
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/aidetableview.hpp
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/aidetreeview.hpp
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/banner.hpp
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/gotittooltip.hpp
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/notificationballoon.hpp
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/searchfilterwidget.hpp
        ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets/searchlineedit.hpp
)

target_sources(
  AideQt${QT_VERSION_MAJOR}Widgets
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/generated_include
      FILES
        ${CMAKE_CURRENT_BINARY_DIR}/generated_include/aide/gui/widgets/aidewidgets_export.h
)

target_include_directories(
  AideQt${QT_VERSION_MAJOR}Widgets
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/
    ${CMAKE_CURRENT_LIST_DIR}/../../include/aide/gui/widgets
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(
  AideQt${QT_VERSION_MAJOR}Widgets
  PUBLIC Qt::Widgets
  PRIVATE
    $<BUILD_LOCAL_INTERFACE:aide_coverage>
    $<BUILD_LOCAL_INTERFACE:aide_options>
    $<BUILD_LOCAL_INTERFACE:aide_warnings>
    Aide::AideLogger
    Aide::AideCore
)

set_source_files_properties(
  ${CMAKE_CURRENT_BINARY_DIR}/AideWidgets_autogen/mocs_compilation.cpp
  PROPERTIES COMPILE_FLAGS "$<$<CXX_COMPILER_ID:GNU>:-Wno-useless-cast>"
)
