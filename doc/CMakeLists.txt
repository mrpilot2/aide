FIND_PACKAGE(Doxygen
  REQUIRED dot)

SET(DOXYGEN_EXCLUDE_PATTERNS
  catch.hpp
)

DOXYGEN_ADD_DOCS(developer_docs
  ${PROJECT_SOURCE_DIR}
  COMMENT "Generate html pages"
)