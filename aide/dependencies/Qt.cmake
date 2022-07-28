find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

find_package(
  Qt6
  COMPONENTS Core Widgets
  QUIET
)

if(TARGET Qt6::Core)
  message(STATUS "Using Qt6")

  find_package(Qt6LinguistTools)
else()
  message(STATUS "Using Qt5")

  find_package(
    Qt5
    COMPONENTS Core Widgets
    REQUIRED
  )

  if(TARGET Qt5::Core)
    set_target_properties(
      Qt5::Core Qt5::Widgets Qt5::moc Qt5::uic Qt5::rcc
      PROPERTIES IMPORTED_GLOBAL TRUE
    )
    if(NOT TARGET Qt::Core)
      add_library(Qt::Core ALIAS Qt5::Core)
      add_library(Qt::Widgets ALIAS Qt5::Widgets)
      add_executable(Qt::moc ALIAS Qt5::moc)
      add_executable(Qt::uic ALIAS Qt5::uic)
      add_executable(Qt::rcc ALIAS Qt5::rcc)
    endif()
  endif()

  find_package(Qt5LinguistTools)

  add_executable(Qt::lrelease ALIAS Qt5::lrelease)
  add_executable(Qt::lupdate ALIAS Qt5::lupdate)
endif()
