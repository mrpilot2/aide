set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_LIST_DIR}/res/forms)

add_library(AideGui OBJECT)
add_library(Aide::AideGui ALIAS AideGui)

target_sources(AideGui PRIVATE mainwindow.cpp)

target_include_directories(
  AideGui PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include
                  ${CMAKE_CURRENT_LIST_DIR}/../include/gui
)

target_link_libraries(AideGui PRIVATE Qt5::Widgets project_coverage)
