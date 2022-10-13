if (OPTION_LANGUAGE_SCAD)
  add_executable(licenses
    licenses.cpp
    ${PROJECT_SOURCE_DIR}/src/orthodocs/globals.cpp
    ${PROJECT_SOURCE_DIR}/src/orthodocs/document.cpp
    ${PROJECT_SOURCE_DIR}/extensions/SCAD/src/scad/document.cpp
  )
  target_include_directories(licenses
    PUBLIC
      ${PROJECT_SOURCE_DIR}/extensions/SCAD/include
      ${PROJECT_SOURCE_DIR}/include
  )
  target_link_libraries(licenses trace)
endif()