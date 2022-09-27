find_program(DIFF NAMES diff REQUIRED)
if (NOT DIFF)
  message(FATAL_ERROR "diff program not found")
endif()

set(TEST_DD_IN ${CMAKE_CURRENT_SOURCE_DIR}/bin/dd.sh.in)
add_subdirectory(flat-lib)
