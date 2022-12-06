file(DOWNLOAD
    https://raw.githubusercontent.com/spdx/license-list-data/main/json/licenses.json
    ${CMAKE_CURRENT_SOURCE_DIR}/licenses.json
)
add_executable(json_test json_test.cpp)
target_link_libraries(json_test
  nlohmann_json::nlohmann_json
)