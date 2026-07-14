include(CheckCXXSourceCompiles)

check_cxx_source_compiles(
  "
  #include <format>
  int main()
  {
    std::format(\"Hello, {}!\", \"world\");
    return 0;
  }
"
  HAVE_CXX20_FORMAT
)

if(NOT HAVE_CXX20_FORMAT)
  message(
    FATAL_ERROR
      "C++20 <format> is required to build Aide. Use a compiler that supports C++20 and has <format> implemented."
  )
endif()
