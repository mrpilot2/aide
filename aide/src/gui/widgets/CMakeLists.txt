add_library(AideWidgets OBJECT)
add_library(Aide::Widgets ALIAS AideWidgets)

target_sources(AideWidgets
    PRIVATE
        searchlineedit.cpp)

target_link_libraries(AideWidgets
    PUBLIC Qt5::Widgets)
