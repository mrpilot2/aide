macro(add_license_dependency)
  set(noValueArgs "")
  set(oneValueArgs PROJECT PROJECT_URL VERSION LICENSE LICENSE_URL)
  set(multiValueArgs "")
  cmake_parse_arguments(
    ALD "${noValueArgs}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN}
  )

  set(table_content
      "${table_content}<tr valign=\"top\"><td class=\"firstColumn\"><a class=\"name\"
  href=\"${ALD_PROJECT_URL}\">${ALD_PROJECT}</a> <span class=\"version\">${ALD_VERSION}</span></td><td
  class=\"secondColumn\"><a
  class=\"license\"
  href=\"${ALD_LICENSE_URL}\">${ALD_LICENSE}</a></td></tr>"
  )
endmacro()
