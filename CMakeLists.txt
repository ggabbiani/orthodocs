# minimum required CMAKE version
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FetchContent)
include(CMakePrintHelpers)
include(GitUtils)

# get GIT_BRANCH_VERSION
git_branch_version()
# cmake_print_variables(GIT_BRANCH_VERSION)

project(OrthoDocs VERSION ${GIT_BRANCH_VERSION})
# cmake_print_variables(PROJECT_VERSION PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_PATCH PROJECT_VERSION_TWEAK)

include(CTest)

option(OPTION_FUNC_TESTS      "Functional tests"              ON  )
option(OPTION_LANGUAGE_SCAD   "OpenSCAD language extension"   ON  )
option(OPTION_OTHER_TESTS     "Other more experimental tests" OFF )
option(OPTION_TRACE           "Debug tracing facility"        OFF )
option(OPTION_WRITER_MARKDOWN "Markdown writer extension"     ON  )

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

FetchContent_Declare(
  cpp-trace
  GIT_REPOSITORY https://ggabbiani@bitbucket.org/ggabbiani/cpp-trace.git
  GIT_TAG        c0dfa4d874e29296680c9b75d3bb833761fe813f # v1.6.2
)
FetchContent_Declare(
  cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        a66ae4145779c56dc0f9f98a631656417dd77de8 # v2.3.0
)
FetchContent_Declare(
  indicators
  GIT_REPOSITORY https://github.com/p-ranav/indicators.git
  GIT_TAG        6432875ef4c866d25e324c19a787b5bb90b0ab12 # v2.2
)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.x
)

FetchContent_MakeAvailable(cpp-trace cli11 indicators spdlog)

if (NOT OPTION_TRACE)
  add_compile_definitions(NTRACE)
endif()

configure_file(include/orthodocs/config.h.in ${PROJECT_SOURCE_DIR}/include/orthodocs/config.h @ONLY)
set(ORTHODOCS_INCS ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ORTHODOCS_LIBS antlr4_static CLI11::CLI11 indicators::indicators trace spdlog::spdlog)

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
