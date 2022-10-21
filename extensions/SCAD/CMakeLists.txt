include_directories(${ANTLR4_INCLUDE_DIRS})
# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
# set(ANTLR_EXECUTABLE /home/user/antlr-4.10.1-complete.jar)
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/tools/antlr-4.10.1-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

###############################################################################
# C++ split parser

# add lexer and grammar to the build dependencies.
antlr_target(${LEXER} ${LEXER}.g4 LEXER PACKAGE scad)
antlr_target(${PARSER}
  ${PARSER}.g4
  PARSER LISTENER
  PACKAGE scad
  DEPENDS_ANTLR ${LEXER}
  COMPILE_FLAGS -lib ${ANTLR_${LEXER}_OUTPUT_DIR}
)

# cmake_print_variables(
#   ANTLR_${LEXER}_CXX_OUTPUTS
#   ANTLR_${PARSER}_CXX_OUTPUTS
#   ANTLR_${LEXER}_OUTPUT_DIR
#   ANTLR_${PARSER}_OUTPUT_DIR
# )

###############################################################################
# C++ client application libs
add_library(${LANGUAGE_LIB} STATIC
  src/scad/annotations.cpp
  src/scad/doc/license.cpp
  src/scad/document.cpp
  src/scad/extension.cpp
  src/scad/listener.cpp
  ${ANTLR_${LEXER}_CXX_OUTPUTS} ${ANTLR_${PARSER}_CXX_OUTPUTS}
)
target_include_directories(${LANGUAGE_LIB}
  PUBLIC
    include
    ${PROJECT_SOURCE_DIR}/include
  PRIVATE
    ${ANTLR_${LEXER}_OUTPUT_DIR} ${ANTLR_${PARSER}_OUTPUT_DIR}
)
target_link_libraries(${LANGUAGE_LIB} PRIVATE antlr4_static trace)
# cmake_print_properties(
#     TARGETS ${LANGUAGE_LIB}
#     PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
# )

###############################################################################
# Java split parser for testing grammars via grun
#
# NOTE: WHEN USING SEPARATED LEXER AND PARSER IT'S EXPECTED THAT THE LEXER ENDS
# ON Lexer AND THE PARSER ENDS ON Parser. THE COMMON PART OF THE FILES IS THE
# NAME OF GRAMMAR. I.E TestLexer AND TestParser -> Test IS THE NAME OF THE
# GRAMMAR TO BE USED WITH GRUN.

find_package(Java QUIET COMPONENTS Runtime REQUIRED)
include(UseJava)

set(JAVA_ARTIFACTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/java)

# Lexer
set(LEXER_GRAMMAR       ${CMAKE_CURRENT_SOURCE_DIR}/${LEXER}.g4 )
set(LEXER_JAVA_SOURCES  ${JAVA_ARTIFACTS_DIR}/${LEXER}.java     )
set(LEXER_JAVA_ARTIFACTS
    ${JAVA_ARTIFACTS_DIR}/${LEXER}.interp
    ${JAVA_ARTIFACTS_DIR}/${LEXER}.tokens
    ${LEXER_JAVA_SOURCES}
)
add_custom_command(
    OUTPUT ${LEXER_JAVA_ARTIFACTS}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
            ${LEXER_GRAMMAR}
            -o ${JAVA_ARTIFACTS_DIR}
            # -no-listener
    DEPENDS ${LEXER_GRAMMAR}
    COMMENT "Building Java Lexer artifacts"
)
# Parser
set(PARSER_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER}.g4)
set(PARSER_JAVA_SOURCES
  ${JAVA_ARTIFACTS_DIR}/${PARSER}.java
  ${JAVA_ARTIFACTS_DIR}/${PARSER}BaseListener.java
  ${JAVA_ARTIFACTS_DIR}/${PARSER}Listener.java
)
set(PARSER_JAVA_ARTIFACTS
    ${JAVA_ARTIFACTS_DIR}/${PARSER}.interp
    ${JAVA_ARTIFACTS_DIR}/${PARSER}.tokens
    ${PARSER_JAVA_SOURCES}
)
add_custom_command(
    OUTPUT ${PARSER_JAVA_ARTIFACTS}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
            ${PARSER_GRAMMAR}
            -o ${JAVA_ARTIFACTS_DIR}
            # -no-listener
    DEPENDS ${LEXER_JAVA_ARTIFACTS} ${PARSER_GRAMMAR}
    COMMENT "Building Java Parser artifacts"
)
add_jar(
    ${LANGUAGE_JAR}
    SOURCES
      ${LEXER_JAVA_SOURCES}
      ${PARSER_JAVA_SOURCES}
    INCLUDE_JARS ${ANTLR_EXECUTABLE}
)

if (OPTION_FUNC_TESTS)
  add_subdirectory(tests-func)
endif()

if (OPTION_OTHER_TESTS)
  add_subdirectory(tests-other)
endif()