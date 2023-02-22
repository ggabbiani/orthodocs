project(spdx_prj VERSION ${GIT_BRANCH_VERSION} LANGUAGES CXX)

###############################################################################
# Java split parser for testing grammars via grun
#
# NOTE: WHEN USING SEPARATED LEXER AND PARSER IT'S EXPECTED THAT THE LEXER ENDS
# ON Lexer AND THE PARSER ENDS ON Parser. THE COMMON PART OF THE FILES IS THE
# NAME OF GRAMMAR. I.E TestLexer AND TestParser -> Test IS THE NAME OF THE
# GRAMMAR TO BE USED WITH GRUN.

find_package(Java COMPONENTS Runtime REQUIRED)
include(UseJava)

set(JAVA_ARTIFACTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/java)
set(LANGUAGE      SPDX)
set(LEXER         ${LANGUAGE}Lexer)
set(PARSER        ${LANGUAGE}Parser)
set(LANGUAGE_LIB  Cpp${LANGUAGE})
set(LANGUAGE_JAR  J${LANGUAGE})
set(LANGUAGE_INCS ${ANTLR4_INCLUDE_DIRS})

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
    COMMENT "Building Java Lexer artifacts for SPDX"
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
    COMMENT "Building Java Parser artifacts for SPDX"
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

###############################################################################
# C++ split parser sources

# add lexer and grammar to the build dependencies.
antlr_target(${LEXER} ${LEXER}.g4
  LEXER
  PACKAGE spdx)
antlr_target(${PARSER}
  ${PARSER}.g4
  PARSER LISTENER
  PACKAGE spdx
  DEPENDS_ANTLR ${LEXER}
  COMPILE_FLAGS -lib ${ANTLR_${LEXER}_OUTPUT_DIR}
)

###############################################################################
# C++ parser for spdx
add_library(${LANGUAGE_LIB} STATIC
  ${ANTLR_${LEXER}_CXX_OUTPUTS} ${ANTLR_${PARSER}_CXX_OUTPUTS}
)
add_library(spdx::parser ALIAS ${LANGUAGE_LIB})
target_include_directories(${LANGUAGE_LIB}
  PUBLIC
    ${ANTLR_${LEXER}_OUTPUT_DIR}
    ${ANTLR_${PARSER}_OUTPUT_DIR}
    ${ANTLR4_INCLUDE_DIRS}
)
target_link_libraries(${LANGUAGE_LIB}
  PUBLIC
    antlr4::runtime
)
