add_library(AideLogger)
add_library(Aide::AideLogger ALIAS AideLogger)

target_sources(AideLogger PRIVATE logger.cpp loggermacros.cpp)

target_sources(
  AideLogger
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include
      FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/loggerinterface.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/logger/loggerfactory.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/logger/log_helper_macros.hpp
)

target_include_directories(
  AideLogger
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/aide/>
)

target_link_libraries(
  AideLogger
  PUBLIC spdlog::spdlog fmt::fmt
  PRIVATE aide_coverage aide_options aide_warnings
)

set(
  AIDE_LOG_LEVEL
  "$<IF:$<CONFIG:Debug>,SPDLOG_LEVEL_TRACE,SPDLOG_LEVEL_INFO>"
  CACHE STRING
  "Choose your desired log level; default for debug builds is TRACE, and for all other builds INFO"
)

set_property(
  CACHE AIDE_LOG_LEVEL
  PROPERTY
    STRINGS
      SPDLOG_LEVEL_TRACE
      SPDLOG_LEVEL_DEBUG
      SPDLOG_LEVEL_INFO,
      SPDLOG_LEVEL_WARN,
      SPDLOG_LEVEL_ERROR,
      SPDLOG_LEVEL_CRITICAL,
      SPDLOG_LEVEL_OFF
)

target_compile_definitions(
  AideLogger
  PUBLIC SPDLOG_ACTIVE_LEVEL=${AIDE_LOG_LEVEL}
)
