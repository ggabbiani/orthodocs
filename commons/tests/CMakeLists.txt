add_executable(json_test json_test.cpp)
target_link_libraries(json_test
  nlohmann_json::nlohmann_json
)

add_executable(parse parse.cpp)
target_link_libraries(parse
  commons
  CLI11::CLI11
)

add_executable(boost_graph1 boost_graph1.cpp)
add_executable(boost_graph2 boost_graph2.cpp)

add_executable(varia_graph1 varia_graph1.cpp)
add_executable(varia_graph2 varia_graph2.cpp)

set_target_properties(varia_graph1 varia_graph2
  PROPERTIES
    INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/include"
)

FetchContent_Declare(
  termcolor
  GIT_REPOSITORY https://github.com/ikalnytskyi/termcolor.git
  GIT_TAG        b3cb0f365f8435588df7a6b12a82b2ac5fc1fe95 # v2.1.0
)

FetchContent_MakeAvailable(termcolor)

add_executable(new-graph new-graph.cpp)
target_link_libraries(new-graph termcolor debug::trace)
