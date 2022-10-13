set(TEST_TARGET   ${PROJECT_BINARY_DIR}/orthodocs)
# set(TEST_SOURCES  "isolated.scad nested.scad undocumented.scad")
set(TEST_SROOT    ${CMAKE_CURRENT_SOURCE_DIR}/src-tree)
set(TEST_DROOT    ${CMAKE_CURRENT_BINARY_DIR}/docs)
set(TEST_OKROOT   ${CMAKE_CURRENT_SOURCE_DIR}/docs)
set(TEST_IGNORE_OKROOT   ${CMAKE_CURRENT_SOURCE_DIR}/docs-ignore)
set(TEST_IGNORE_DROOT    ${CMAKE_CURRENT_BINARY_DIR}/docs-ignore)

configure_file(docs.cmake.in docs.cmake @ONLY)
add_test(
  NAME document-flat-tree
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/docs.cmake
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
set_tests_properties(document-flat-tree PROPERTIES RUN_SERIAL TRUE)

configure_file(docs-ignore.cmake.in docs-ignore.cmake @ONLY)
add_test(
  NAME document-ignore-flat-tree
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/docs-ignore.cmake
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)
set_tests_properties(document-ignore-flat-tree PROPERTIES RUN_SERIAL TRUE)
