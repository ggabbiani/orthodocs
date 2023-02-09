# functional tests for the current language extension ${LANGUAGE_LIB}

# parse tests
function(parse_spdx)
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
  # cmake_print_variables(TEST_NAME CLASS_PATH)
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
  set_tests_properties(${TEST_NAME}
    PROPERTIES
      RUN_SERIAL TRUE
      LABELS SPDX
  )
endfunction(parse_spdx)

parse_spdx(NAME spdx-id           RULE all FILE id.spdx             FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-beyond       RULE all FILE id_and_beyond.spdx  FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-nothing      RULE all FILE nothing.spdx        FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-text-after   RULE all FILE text-after.anno     FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-text-before  RULE all FILE text-before.anno    FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-text-after-and-before  RULE all FILE text-after-and-before.anno  FAIL_REGULAR_EXPRESSION DEFAULT)
# taken from [spdx-tutorial/README.md](https://github.com/david-a-wheeler/spdx-tutorial/blob/master/README.md)
parse_spdx(NAME spdx-expr-AND     RULE all FILE expr_AND.spdx       FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-expr-OR      RULE all FILE expr_OR.spdx        FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-expr-order   RULE all FILE expr_order.spdx     FAIL_REGULAR_EXPRESSION DEFAULT)
parse_spdx(NAME spdx-expr-WITH    RULE all FILE expr_WITH.spdx      FAIL_REGULAR_EXPRESSION DEFAULT)
