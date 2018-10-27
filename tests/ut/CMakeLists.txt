ADD_EXECUTABLE(aide_ut
  main.cpp
)

TARGET_SOURCES(aide_ut
  PRIVATE
    applicationtest.cpp
    idtest.cpp
)

ADD_LIBRARY(catch INTERFACE)

TARGET_INCLUDE_DIRECTORIES(catch
  INTERFACE
    catch
)

TARGET_COMPILE_FEATURES(catch INTERFACE cxx_constexpr)

TARGET_COMPILE_DEFINITIONS(aide_ut
  PRIVATE
    $<$<CXX_COMPILER_ID:Clang>:CATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS>
)

TARGET_LINK_LIBRARIES(aide_ut
  PRIVATE
    catch
    aide
)

INCLUDE(../../cmake/Catch.cmake)

CATCH_DISCOVER_TESTS(aide_ut)
