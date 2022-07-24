set(LANGUAGE SCAD)
set(LEXER ${LANGUAGE}Lexer)
set(PARSER ${LANGUAGE}Parser)

# Call macro to add lexer and grammar to your build dependencies.
antlr_target(${LEXER} ${LEXER}.g4 LEXER PACKAGE scad)
antlr_target(${PARSER} ${PARSER}.g4 PARSER
            PACKAGE scad
            DEPENDS_ANTLR ${LEXER}
            COMPILE_FLAGS -lib ${ANTLR_${LEXER}_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_${LEXER}_OUTPUT_DIR})
include_directories(${ANTLR_${PARSER}_OUTPUT_DIR})

###############################################################################
# executables
add_executable(scad main.cpp ${ANTLR_${LEXER}_CXX_OUTPUTS} ${ANTLR_${PARSER}_CXX_OUTPUTS})
target_link_libraries(scad antlr4_static)

###############################################################################
# Java parser for testing via grun
find_package(Java QUIET COMPONENTS Runtime REQUIRED)
include(UseJava)

set(JAVA_ARTIFACTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/java)
# Lexer
set(LEXER_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/${LEXER}.g4)
set(LEXER_JAVA_SOURCES ${JAVA_ARTIFACTS_DIR}/${LEXER}.java )
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
            -no-listener
    DEPENDS ${LEXER_GRAMMAR}
    COMMENT "Building Java Lexer artifacts"
)
# Parser
set(PARSER_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/${PARSER}.g4)
set(PARSER_JAVA_SOURCES ${JAVA_ARTIFACTS_DIR}/${PARSER}.java)
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
            -no-listener
    DEPENDS ${LEXER_JAVA_ARTIFACTS} ${PARSER_GRAMMAR}
    COMMENT "Building Java Parser artifacts"
)
add_jar(
    ${LANGUAGE}
    SOURCES ${LEXER_JAVA_SOURCES} ${PARSER_JAVA_SOURCES}
    INCLUDE_JARS ${ANTLR_EXECUTABLE}
)

###############################################################################
# ctest(s)

if (BUILD_TESTING)
  set(TESTER_EXEC tester)
  add_executable(${TESTER_EXEC} tester.cpp ${ANTLR_${LEXER}_CXX_OUTPUTS} ${ANTLR_${PARSER}_CXX_OUTPUTS})
  target_link_libraries(${TESTER_EXEC} antlr4_static)

  add_test(
    NAME error
    COMMAND ${TESTER_EXEC} error.scad
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
  )
  set_property(TEST error PROPERTY WILL_FAIL true)

   add_test(
    NAME assignments
    COMMAND ${TESTER_EXEC} assignment.scad
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests
  )
endif()

###############################################################################
# sub directories
add_subdirectory(bin)
