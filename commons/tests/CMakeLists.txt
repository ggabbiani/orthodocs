add_executable(json_test json_test.cpp)
target_link_libraries(json_test
  nlohmann_json::nlohmann_json
)

add_executable(parse parse.cpp)
target_link_libraries(parse
  commons
  CLI11::CLI11
)

add_executable(varia_graph1 varia_graph1.cpp)
target_link_libraries(varia_graph1 PRIVATE commons)
add_executable(varia_graph2 varia_graph2.cpp)
target_link_libraries(varia_graph2 PRIVATE commons)

# set_target_properties(varia_graph1 varia_graph2
#   PROPERTIES
#     INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include"
# )

# add_executable(new-graph new-graph.cpp)
# target_link_libraries(new-graph nlohmann_json::nlohmann_json termcolor cpp::trace)
