# include(FindUnixCommands)

find_program(DIFF NAMES diff REQUIRED)
if (NOT DIFF)
  message(FATAL_ERROR "diff program not found")
endif()

add_executable(test.bin test.cpp)
target_link_libraries(test.bin trace)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(DESIRED_RESULT ${CMAKE_CURRENT_SOURCE_DIR}/desired-result.txt)
else()
  set(DESIRED_RESULT ${CMAKE_CURRENT_SOURCE_DIR}/dummy-result.txt)
endif()
set(TEST_RESULT ${CMAKE_CURRENT_BINARY_DIR}/test-result.txt)
set(TEST_BIN ${CMAKE_CURRENT_BINARY_DIR}/test.bin)
configure_file(testdiff.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/testdiff.cmake @ONLY)

add_test(
  NAME test
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/testdiff.cmake --verbose
)