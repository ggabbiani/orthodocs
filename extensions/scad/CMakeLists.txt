set(LANGUAGE SCAD)
# set(LEXER ${LANGUAGE}Lexer)
# set(PARSER ${LANGUAGE}Parser)

find_package(Boost COMPONENTS system filesystem REQUIRED)

###############################################################################
# 1) C++ parser

# Call macro to add lexer and grammar to your build dependencies.
# antlr_target(${LEXER} ${LEXER}.g4 LEXER PACKAGE scad)
# antlr_target(${PARSER} ${PARSER}.g4 PARSER LISTENER
#             PACKAGE scad
#             DEPENDS_ANTLR ${LEXER}
#             COMPILE_FLAGS -lib ${ANTLR_${LEXER}_OUTPUT_DIR})

# Lexer + Parser
antlr_target(${LANGUAGE} ${LANGUAGE}.g4 LEXER PARSER LISTENER PACKAGE scad)

# include generated files in project environment
# include_directories(${ANTLR_${LEXER}_OUTPUT_DIR})
# include_directories(${ANTLR_${PARSER}_OUTPUT_DIR})

# include_directories(${ANTLR_${LANGUAGE}_OUTPUT_DIR})
message("ANTLR_${LANGUAGE}_OUTPUT_DIR: ${ANTLR_${LANGUAGE}_OUTPUT_DIR}")

###############################################################################
# 2) C++ application using the parser at 1)
# add_executable(scad main.cpp ${ANTLR_${LEXER}_CXX_OUTPUTS} ${ANTLR_${PARSER}_CXX_OUTPUTS})
# target_link_libraries(scad antlr4_static ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
set(LIB_NAME scad)
add_library(${LIB_NAME} STATIC
  src/scad/annotations.cpp 
  src/scad/extension.cpp
  src/scad/document.cpp 
  src/scad/listener.cpp
  ${ANTLR_${LANGUAGE}_CXX_OUTPUTS}
)
target_include_directories(${LIB_NAME}
  PUBLIC
    ${PROJECT_SOURCE_DIR}/include 
    include
    ${ANTLR_${LANGUAGE}_OUTPUT_DIR}
)
# target_compile_options(${LIB_NAME} PUBLIC "-Wl,-u,scad_loader")
target_link_libraries(${LIB_NAME} antlr4_static)

###############################################################################
# 3) C++ application for annotation style testing

# add_executable(style-tester
#   src/style-tester.cpp 
#   src/globals.cpp 
#   src/utils.cpp 
#   src/document.cpp
# )
# target_include_directories(style-tester PUBLIC include)
# target_link_libraries(style-tester)

###############################################################################
# 4) Java parser for testing grammars via grun
find_package(Java QUIET COMPONENTS Runtime REQUIRED)
include(UseJava)

set(JAVA_ARTIFACTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/java)
# Lexer
# set(LEXER_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/${LEXER}.g4)
# set(LEXER_JAVA_SOURCES ${JAVA_ARTIFACTS_DIR}/${LEXER}.java )
# set(LEXER_JAVA_ARTIFACTS
#     ${JAVA_ARTIFACTS_DIR}/${LEXER}.interp
#     ${JAVA_ARTIFACTS_DIR}/${LEXER}.tokens
#     ${LEXER_JAVA_SOURCES}
# )
# add_custom_command(
#     OUTPUT ${LEXER_JAVA_ARTIFACTS}
#     COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
#             ${LEXER_GRAMMAR}
#             -o ${JAVA_ARTIFACTS_DIR}
#             # -no-listener
#     DEPENDS ${LEXER_GRAMMAR}
#     COMMENT "Building Java Lexer artifacts"
# )
# # Parser
# set(PARSER_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER}.g4)
# set(PARSER_JAVA_SOURCES ${JAVA_ARTIFACTS_DIR}/${PARSER}.java ${JAVA_ARTIFACTS_DIR}/${PARSER}BaseListener.java ${JAVA_ARTIFACTS_DIR}/${PARSER}Listener.java)
# set(PARSER_JAVA_ARTIFACTS
#     ${JAVA_ARTIFACTS_DIR}/${PARSER}.interp
#     ${JAVA_ARTIFACTS_DIR}/${PARSER}.tokens
#     ${PARSER_JAVA_SOURCES}
# )
# add_custom_command(
#     OUTPUT ${PARSER_JAVA_ARTIFACTS}
#     COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
#             ${PARSER_GRAMMAR}
#             -o ${JAVA_ARTIFACTS_DIR}
#             # -no-listener
#     DEPENDS ${LEXER_JAVA_ARTIFACTS} ${PARSER_GRAMMAR}
#     COMMENT "Building Java Parser artifacts"
# )

# Lexer + Parser
set(LANGUAGE_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/${LANGUAGE}.g4)
set(LANGUAGE_JAVA_SOURCES
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}BaseListener.java
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}Lexer.java
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}Listener.java
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}Parser.java
)
set(LANGUAGE_JAVA_ARTIFACTS
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}.interp
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}Lexer.interp
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}Lexer.tokens
  ${JAVA_ARTIFACTS_DIR}/${LANGUAGE}.tokens
  ${LANGUAGE_JAVA_SOURCES}
)
add_custom_command(
    OUTPUT ${LANGUAGE_JAVA_ARTIFACTS}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
            ${LANGUAGE_GRAMMAR}
            -o ${JAVA_ARTIFACTS_DIR}
    DEPENDS ${LANGUAGE_GRAMMAR}
    COMMENT "Building Java artifacts"
)

add_jar(
    ${LANGUAGE}
    SOURCES ${LANGUAGE_JAVA_SOURCES}
    INCLUDE_JARS ${ANTLR_EXECUTABLE}
)

###############################################################################
# ctest(s)
configure_file(grun.in grun USE_SOURCE_PERMISSIONS @ONLY)
set(GRUN ${CMAKE_CURRENT_BINARY_DIR}/grun)

function(new_test)
  set(options )
  set(oneValueArgs NAME RULE FILE)
  set(multiValueArgs PASS_REGULAR_EXPRESSION FAIL_REGULAR_EXPRESSION)
  cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  set(test_default "^line")
  add_test(NAME "${TEST_NAME}" COMMAND "${GRUN}" "${LANGUAGE}" "${TEST_RULE}" "${TEST_FILE}" WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/parse-tests")
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
  set_property(TEST "${TEST_NAME}" PROPERTY RUN_SERIAL TRUE)
endfunction(new_test)

if (BUILD_TESTING)
  new_test(NAME comments      RULE pkg  FILE comments.scad      FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME uses          RULE pkg  FILE uses.scad          FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME vectors       RULE pkg  FILE vectors.scad       FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME error         RULE pkg  FILE error.scad         PASS_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME assignments   RULE pkg  FILE assignment.scad    FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME modules       RULE pkg  FILE modules.scad       FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME includes      RULE pkg  FILE includes.scad      FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME all           RULE pkg  FILE all.scad           FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME func_lit      RULE function_literal FILE func_lit.scad  FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME assert        RULE pkg  FILE assert.scad        FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME if_statement  RULE pkg  FILE if_statement.scad  FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME lc_generators RULE pkg  FILE lc_generators.scad FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME lc_for        RULE pkg  FILE lc_for.scad        FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME lc_advanced   RULE pkg  FILE lc_advanced.scad   FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME statement_for RULE pkg  FILE statement_for.scad FAIL_REGULAR_EXPRESSION DEFAULT)
  new_test(NAME vars          RULE pkg  FILE vars.scad          FAIL_REGULAR_EXPRESSION DEFAULT)
endif()

add_subdirectory("tests-other")