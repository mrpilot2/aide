FIND_PACKAGE(Doxygen
  COMPONENTS dot)

IF (DOXYGEN_FOUND)
  SET(DOXYGEN_EXCLUDE_PATTERNS
    catch.hpp
  )

  DOXYGEN_ADD_DOCS(developer_docs
    ${PROJECT_SOURCE_DIR}
    COMMENT "Generate html pages"
  )
ENDIF()