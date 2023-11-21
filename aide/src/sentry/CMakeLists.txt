add_library(AideSentry OBJECT)
add_library(Aide::AideSentry ALIAS AideSentry)

target_sources(AideSentry PRIVATE sentrycleanup.cpp)

if(ENABLE_SENTRY_INTEGRATION)
  target_sources(AideSentry PRIVATE sentry.cpp)
else()
  target_sources(AideSentry PRIVATE nullsentry.cpp)
endif()

target_include_directories(
  AideSentry PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
                    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)

target_link_libraries(AideSentry PUBLIC aide_project_sanitizers)

if(ENABLE_SENTRY_INTEGRATION)
  target_link_libraries(
    AideSentry
    PUBLIC sentry::sentry sentry-breakpad::sentry-breakpad
    PRIVATE Qt::Core
  )
endif()

set_target_properties(AideSentry PROPERTIES POSITION_INDEPENDENT_CODE ON)
