FIND_PACKAGE(Doxygen
  REQUIRED dot)

DOXYGEN_ADD_DOCS(developer_docs
  ${PROJECT_SOURCE_DIR}
  COMMENT "Generate html pages"
)