# functional tests for the current language extension ${LANGUAGE_LIB}

# parse tests
function(parse_test)
  set(options )
  set(oneValueArgs NAME RULE FILE)
  set(multiValueArgs PASS_REGULAR_EXPRESSION FAIL_REGULAR_EXPRESSION)
  cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  set(TEST_NAME parse-${TEST_NAME})
  set(test_default "^line")
  # the CLASS_PATH has a different separator in Windows so we set it accordingly
  get_target_property(JAR_DIRECTORY ${LANGUAGE_JAR} BINARY_DIR)
  if (WIN32)
    set(CLASS_PATH "${JAR_DIRECTORY}/${LANGUAGE_JAR}.jar;${CMAKE_HOME_DIRECTORY}/tools/antlr-4.10.1-complete.jar")
  else()
    set(CLASS_PATH "${JAR_DIRECTORY}/${LANGUAGE_JAR}.jar:${CMAKE_HOME_DIRECTORY}/tools/antlr-4.10.1-complete.jar")
  endif()
  add_test(
    NAME ${TEST_NAME}
    COMMAND "${Java_JAVA_EXECUTABLE}" -cp "${CLASS_PATH}" -Xmx500M org.antlr.v4.gui.TestRig "${LANGUAGE}" "${TEST_RULE}" "${TEST_FILE}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/parse-tests"
  )
  if (DEFINED TEST_PASS_REGULAR_EXPRESSION)
    if (TEST_PASS_REGULAR_EXPRESSION STREQUAL "DEFAULT")
      set(value ${test_default})
    else()
      set(value ${TEST_PASS_REGULAR_EXPRESSION})
    endif()
    set_property(TEST "${TEST_NAME}" PROPERTY PASS_REGULAR_EXPRESSION "${value}")
    # message("${TEST_NAME} TEST_PASS_REGULAR_EXPRESSION: ${TEST_PASS_REGULAR_EXPRESSION} (${value})")
  endif()
  if (DEFINED TEST_FAIL_REGULAR_EXPRESSION)
    if (TEST_FAIL_REGULAR_EXPRESSION STREQUAL "DEFAULT")
      set(value ${test_default})
    else()
      set(value ${TEST_FAIL_REGULAR_EXPRESSION})
    endif()
    set_property(TEST "${TEST_NAME}" PROPERTY FAIL_REGULAR_EXPRESSION "${value}")
    # message("${TEST_NAME} TEST_FAIL_REGULAR_EXPRESSION: ${TEST_FAIL_REGULAR_EXPRESSION} (${value})")
  endif()
  set_tests_properties(${TEST_NAME} PROPERTIES RUN_SERIAL TRUE)
endfunction(parse_test)

# annotation style recognition tests
function(style_recon_test)
  set(options)
  set(oneValueArgs STYLE)
  set(multiValueArgs FILES)
  cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  set(TEST_NAME style-recon-${TEST_STYLE})
  add_test(
    NAME ${TEST_NAME}
    COMMAND style-factory-test -s annotation-styles -e ${TEST_STYLE} ${TEST_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  set_tests_properties(${TEST_NAME} PROPERTIES RUN_SERIAL TRUE)
endfunction(style_recon_test)

add_executable(style-factory-test
  style-factory-test.cpp
  ${PROJECT_SOURCE_DIR}/src/orthodocs/globals.cpp
  ${PROJECT_SOURCE_DIR}/src/orthodocs/utils.cpp
  ${PROJECT_SOURCE_DIR}/src/orthodocs/document.cpp
)
target_include_directories(style-factory-test PUBLIC ${ORTHODOCS_INCS})
target_link_libraries(style-factory-test ${LANGUAGE_LIB} CLI11::CLI11 trace)

parse_test(NAME all           RULE pkg  FILE all.scad           FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME assert        RULE pkg  FILE assert.scad        FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME assignments   RULE pkg  FILE assignment.scad    FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME comments      RULE pkg  FILE comments.scad      FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME error         RULE pkg  FILE error.scad         PASS_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME func_lit      RULE function_literal FILE func_lit.scad  FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME if_statement  RULE pkg  FILE if_statement.scad  FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME includes      RULE pkg  FILE includes.scad      FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME lc_advanced   RULE pkg  FILE lc_advanced.scad   FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME lc_generators RULE pkg  FILE lc_generators.scad FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME lc_for        RULE pkg  FILE lc_for.scad        FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME statement_for RULE pkg  FILE statement_for.scad FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME modules       RULE pkg  FILE modules.scad       FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME uses          RULE pkg  FILE uses.scad          FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME vars          RULE pkg  FILE vars.scad          FAIL_REGULAR_EXPRESSION DEFAULT)
parse_test(NAME vectors       RULE pkg  FILE vectors.scad       FAIL_REGULAR_EXPRESSION DEFAULT)

style_recon_test(STYLE SINGLE FILES style-single.anno)
style_recon_test(STYLE SIMPLE FILES style-simple0.anno style-simple1.anno style-simple2.anno)
style_recon_test(STYLE FINE   FILES style-fine.anno style-fine1.anno)
