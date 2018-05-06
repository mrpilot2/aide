ADD_EXECUTABLE(aide_ut
  main.cpp
)

TARGET_SOURCES(aide_ut
  PRIVATE
    applicationtest.cpp
)

TARGET_INCLUDE_DIRECTORIES(aide_ut
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/catch
)
