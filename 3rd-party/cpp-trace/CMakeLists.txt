cmake_minimum_required(VERSION 3.22.2 FATAL_ERROR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CMakePrintHelpers)

project(cpp-trace VERSION 1.1.0 LANGUAGES CXX)
cmake_print_variables(PROJECT_NAME PROJECT_VERSION)

option(OPTION_FUNC_TESTS      "Functional tests"              ON  )

include(CTest)

###############################################################################
# the library

find_package(termcolor REQUIRED)
cmake_print_variables(termcolor_FOUND)

add_library(trace STATIC
  src/trace.cpp
)
target_include_directories(trace BEFORE PUBLIC include)

if (OPTION_FUNC_TESTS)
  add_subdirectory(tests-func)
endif()
