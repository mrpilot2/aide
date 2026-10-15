find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets GLOBAL)

find_package(Qt6LinguistTools)

# Qt is LGPL-licensed: link it dynamically so end users can swap the Qt
# libraries. Consumers with a commercial Qt license may opt out.
option(AIDE_ALLOW_STATIC_QT "Allow linking against a static Qt build" OFF)

get_target_property(_aide_qt_core_type Qt6::Core TYPE)
if(_aide_qt_core_type STREQUAL "STATIC_LIBRARY" AND NOT AIDE_ALLOW_STATIC_QT)
  message(
    FATAL_ERROR
    "Qt6::Core is a static library. aIDE requires a shared Qt build to "
    "comply with the Qt LGPL license. Set AIDE_ALLOW_STATIC_QT=ON to override "
    "(e.g. with a commercial Qt license)."
  )
endif()
unset(_aide_qt_core_type)
