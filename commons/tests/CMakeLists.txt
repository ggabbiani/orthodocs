add_executable(json_test json_test.cpp)
target_link_libraries(json_test
  nlohmann_json::nlohmann_json
)

add_executable(parse parse.cpp)
target_link_libraries(parse
  commons
  CLI11::CLI11
)
