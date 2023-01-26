add_executable(nocase-rfind nocase-rfind.cpp)

add_executable(commentdb commentdb.cpp)
target_include_directories(commentdb PRIVATE
  ${ANTLR_${LEXER}_OUTPUT_DIR}
)
target_link_libraries(commentdb PRIVATE
  language::scad
)