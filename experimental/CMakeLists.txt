if (OPTION_LANGUAGE_SCAD)
  add_executable(licenses
    licenses.cpp
    ${PROJECT_SOURCE_DIR}/src/orthodocs/globals.cpp
    ${PROJECT_SOURCE_DIR}/src/orthodocs/document.cpp
    ${PROJECT_SOURCE_DIR}/extensions/scad/src/scad/document.cpp
  )
  target_include_directories(licenses
    PUBLIC
      ${PROJECT_SOURCE_DIR}/extensions/scad/include
      ${PROJECT_SOURCE_DIR}/include
  )
endif()