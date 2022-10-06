set(TEST_DD ${CMAKE_CURRENT_BINARY_DIR}/dd.sh)
set(TEST_TARGET ${PROJECT_BINARY_DIR}/orthodocs)
set(TEST_SOURCES "isolated.scad nested.scad undocumented.scad")
set(TEST_SROOT ${CMAKE_CURRENT_SOURCE_DIR}/src-tree)
set(TEST_DROOT ${CMAKE_CURRENT_BINARY_DIR}/doc-root)
set(TEST_OKROOT ${CMAKE_CURRENT_SOURCE_DIR}/doc-root)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PARENT)
#cmake_path(APPEND ${PARENT} testdiff.cmake.in [OUTPUT_VARIABLE <out-var>])
configure_file(${PARENT}/testdiff.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/testdiff.cmake @ONLY)
add_test(
  NAME flat-lib
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/testdiff.cmake
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)