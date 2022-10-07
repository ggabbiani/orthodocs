find_program(DIFF NAMES diff REQUIRED)
if (NOT DIFF)
  message(FATAL_ERROR "diff program not found")
endif()

add_subdirectory(flat-lib)
