# minimum required CMAKE version
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(CMakePrintHelpers)
include(GitUtils)
# get GIT_BRANCH_VERSION
git_branch_version()
# cmake_print_variables(GIT_BRANCH_VERSION)

project(OrthoDocs VERSION ${GIT_BRANCH_VERSION})
# cmake_print_variables(PROJECT_VERSION PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH PROJECT_VERSION_TWEAK)

option(OPTION_FUNC_TESTS      "Functional tests"              ON  )
option(OPTION_LANGUAGE_SCAD   "OpenSCAD language extension"   ON  )
option(OPTION_OTHER_TESTS     "Other more experimental tests" OFF )
option(OPTION_WRITER_MARKDOWN "Markdown writer extension"     ON  )

include(CTest)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# disabling GNU extension make cmake use -std=c++17 and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

###############################################################################
# global propedeutics for antrl4cpp
###############################################################################
# required if linking to static library
add_definitions(-DANTLR4CPP_STATIC)

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
# set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
# set(ANTLR4_TAG 4.10.1)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.10.1.zip)
set(ANTLR4_ZIP_REPOSITORY ${PROJECT_SOURCE_DIR}/tools/antlr4-4.10.1.zip)
# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
# include_directories(${ANTLR4_INCLUDE_DIRS})
# cmake_print_variables(ANTLR4_INCLUDE_DIRS)
###############################################################################

###############################################################################
# global propedeutics for orthodocs
###############################################################################
add_subdirectory(3rd-party/cpp-trace)

configure_file(include/orthodocs/config.h.in ${PROJECT_SOURCE_DIR}/include/orthodocs/config.h @ONLY)
set(ORTHODOCS_INCS ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party/cpp-trace/include)
set(ORTHODOCS_LIBS antlr4_static trace)

if (OPTION_LANGUAGE_SCAD)
  set(LANGUAGE      SCAD)
  set(LEXER         ${LANGUAGE}Lexer)
  set(PARSER        ${LANGUAGE}Parser)
  set(LANGUAGE_LIB  Cpp${LANGUAGE})
  set(LANGUAGE_JAR  J${LANGUAGE})
  set(LANGUAGE_INCS ${ANTLR4_INCLUDE_DIRS})

  add_subdirectory(extensions/${LANGUAGE})
  list(APPEND ORTHODOCS_LIBS ${LANGUAGE_LIB})
endif()

if (OPTION_WRITER_MARKDOWN)
  set(WRITER      Markdown)
  set(WRITER_LIB  Markdown)

  add_subdirectory(extensions/${WRITER})
  list(APPEND ORTHODOCS_LIBS ${WRITER_LIB})
endif()
# cmake_print_variables(ORTHODOCS_INCS ORTHODOCS_LIBS)

add_executable(orthodocs
  src/orthodocs/analizer.cpp
  src/orthodocs/document.cpp
  src/orthodocs/extensions.cpp
  src/orthodocs/globals.cpp
  src/orthodocs/main.cpp
  src/orthodocs/utils.cpp
  src/orthodocs/xref.cpp
)
target_include_directories(orthodocs PUBLIC ${ORTHODOCS_INCS})
target_link_libraries(orthodocs ${ORTHODOCS_LIBS})
# cmake_print_properties(
#     TARGETS orthodocs
#     PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
# )

if (OPTION_OTHER_TESTS)
  add_subdirectory(tests-other)
endif()

if (OPTION_FUNC_TESTS)
  add_subdirectory(tests-func)
endif()
