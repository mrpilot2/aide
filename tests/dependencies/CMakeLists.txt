find_package(Catch2 REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

find_package(
  Qt6
  COMPONENTS Core Widgets Test
  QUIET
)

if(TARGET Qt6::Test)
  find_package(Qt6LinguistTools)
else()
  find_package(
    Qt5
    COMPONENTS Widgets Test
    REQUIRED
  )

  if(NOT TARGET Qt::Test)
    add_library(Qt::Test ALIAS Qt5::Test)
  elseif(NOT TARGET Qt::Widgets)
    add_library(Qt::Widgets ALIAS Qt5::Widgets)
  endif()

  find_package(Qt5LinguistTools)
  add_executable(Qt::lrelease ALIAS Qt5::lrelease)
  add_executable(Qt::lupdate ALIAS Qt5::lupdate)
endif()
