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