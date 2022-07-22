# Call macro to add lexer and grammar to your build dependencies.
antlr_target(SCADLexer SCADLexer.g4 LEXER PACKAGE scad)
antlr_target(SCADParser SCADParser.g4 PARSER
            PACKAGE scad
            DEPENDS_ANTLR SCADLexer
            COMPILE_FLAGS -lib ${ANTLR_SCADLexer_OUTPUT_DIR})

# include generated files in project environment
include_directories(${ANTLR_SCADLexer_OUTPUT_DIR})
include_directories(${ANTLR_SCADParser_OUTPUT_DIR})

# add generated grammar to demo binary target
add_executable(scad main.cpp ${ANTLR_SCADLexer_CXX_OUTPUTS} ${ANTLR_SCADParser_CXX_OUTPUTS})
target_link_libraries(scad antlr4_static)

# defines Java targets for grammar testing and development
find_package(Java QUIET COMPONENTS Runtime REQUIRED)
include(UseJava)

set(JAVA_ARTIFACTS_DIR ${CMAKE_BINARY_DIR}/java)
# Lexer
set(LEXER_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/SCADLexer.g4)
set(LEXER_JAVA_SOURCES ${JAVA_ARTIFACTS_DIR}/SCADLexer.java )
set(LEXER_JAVA_ARTIFACTS
    ${JAVA_ARTIFACTS_DIR}/SCADLexer.interp
    ${JAVA_ARTIFACTS_DIR}/SCADLexer.tokens
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
set(PARSER_GRAMMAR ${CMAKE_CURRENT_SOURCE_DIR}/SCADParser.g4)
set(PARSER_JAVA_SOURCES ${JAVA_ARTIFACTS_DIR}/SCADParser.java ${JAVA_ARTIFACTS_DIR}/SCADParserBaseListener.java ${JAVA_ARTIFACTS_DIR}/SCADParserListener.java)
set(PARSER_JAVA_ARTIFACTS
    ${JAVA_ARTIFACTS_DIR}/SCADParser.interp
    ${JAVA_ARTIFACTS_DIR}/SCADParser.tokens
    ${PARSER_JAVA_SOURCES}
)
add_custom_command(
    OUTPUT ${PARSER_JAVA_ARTIFACTS}
    COMMAND ${Java_JAVA_EXECUTABLE} -jar ${ANTLR_EXECUTABLE}
            ${PARSER_GRAMMAR}
            -o ${JAVA_ARTIFACTS_DIR}
            -no-listener
    DEPENDS ${LEXER_GRAMMAR} ${PARSER_GRAMMAR}
    COMMENT "Building Java Parser artifacts"
)
add_jar(
    SCAD
    SOURCES ${LEXER_JAVA_SOURCES} ${PARSER_JAVA_SOURCES}
    INCLUDE_JARS ${ANTLR_EXECUTABLE}
)