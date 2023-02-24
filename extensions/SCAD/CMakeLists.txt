include_directories(${ANTLR4_INCLUDE_DIRS})

###############################################################################
# C++ split parser

# add lexer and grammar to the build dependencies.
antlr_target(${LEXER} ${LEXER}.g4 LEXER PACKAGE scad)
antlr_target(${PARSER}
  ${PARSER}.g4
  PARSER LISTENER VISITOR
  PACKAGE scad
  DEPENDS_ANTLR ${LEXER}
  COMPILE_FLAGS -lib ${ANTLR_${LEXER}_OUTPUT_DIR}
)

###############################################################################
# C++ client application lib
add_library(${LANGUAGE_LIB} STATIC
  src/annotations.cpp
  src/document.cpp
  src/extension.cpp
  src/listener.cpp
  $<$<BOOL:${ODOX_SCAD_VISITORS}>:src/visitor.cpp>
  ${ANTLR_${LEXER}_CXX_OUTPUTS}
  ${ANTLR_${PARSER}_CXX_OUTPUTS}
)
add_library(language::scad ALIAS ${LANGUAGE_LIB})
target_include_directories(${LANGUAGE_LIB}
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    src
    ${ANTLR_${LEXER}_OUTPUT_DIR}
    ${ANTLR_${PARSER}_OUTPUT_DIR}
)
target_link_libraries(${LANGUAGE_LIB}
  PUBLIC
    commons
    antlr4::runtime
)

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

if (ODOX_FUNC_TESTS)
  add_subdirectory(tests-func)
endif()

if (ODOX_OTHER_TESTS)
  add_subdirectory(tests-other)
endif()