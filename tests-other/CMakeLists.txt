if (OPTION_LANGUAGE_SCAD)
  add_executable(licenses
    licenses.cpp
    ${PROJECT_SOURCE_DIR}/src/globals.cpp
    ${PROJECT_SOURCE_DIR}/src/document.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
  )
  target_include_directories(licenses
    PRIVATE
      ${PROJECT_SOURCE_DIR}/extensions/SCAD/src
      ${PROJECT_SOURCE_DIR}/src
  )
  target_link_libraries(licenses spdlog::spdlog ${LANGUAGE_LIB})

  add_executable(xref
    xref.cpp
    ${PROJECT_SOURCE_DIR}/src/globals.cpp
    ${PROJECT_SOURCE_DIR}/src/document.cpp
    ${PROJECT_SOURCE_DIR}/src/utils.cpp
  )
  target_include_directories(xref
    PUBLIC
      ${PROJECT_SOURCE_DIR}/extensions/SCAD/src
      ${PROJECT_SOURCE_DIR}/src
  )
  target_link_libraries(xref spdlog::spdlog ${LANGUAGE_LIB})

endif()