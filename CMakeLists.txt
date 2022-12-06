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
option(OPTION_UNIT_TESTS      "Unit tests"                    ON  )
option(OPTION_WRITER_MARKDOWN "Markdown writer extension"     ON  )

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# disabling GNU extension make cmake use -std=c++17 and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

###############################################################################
# propedeutics for antrl4cpp
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
# propedeutics for orthodocs
###############################################################################

FetchContent_Declare(cli11
  GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
  GIT_TAG        a66ae4145779c56dc0f9f98a631656417dd77de8 # v2.3.0
)
FetchContent_Declare(indicators
  GIT_REPOSITORY https://github.com/p-ranav/indicators.git
  GIT_TAG        6432875ef4c866d25e324c19a787b5bb90b0ab12 # v2.2
)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.x
)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
# cmake_print_variables(Boost_INCLUDE_DIRS dtl_SOURCE_DIR)

find_package(Python3 3.10)
cmake_print_variables(Python3_FOUND)
if (NOT Python3_FOUND)
  message(WARNING "Python3 version >= 3.10 required: no version bump and unit test possible")
endif()

FetchContent_MakeAvailable(cli11 json indicators spdlog)

###############################################################################
# commons
###############################################################################
add_library(commons
  src/analizer.cpp
  src/document.cpp
  src/globals.cpp
  src/utils.cpp
)
target_include_directories(commons PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(commons
  spdlog::spdlog
  indicators::indicators
)
# cmake_print_properties(
#     TARGETS orthodocs
#     PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
# )

###############################################################################
# SCAD language extension
###############################################################################
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

###############################################################################
# Markdown writer extension
###############################################################################
if (OPTION_WRITER_MARKDOWN)
  set(WRITER      Markdown)
  set(WRITER_LIB  Markdown)

  add_subdirectory(extensions/${WRITER})
  list(APPEND ORTHODOCS_LIBS ${WRITER_LIB})
endif()

###############################################################################
# factories
###############################################################################
add_library(factories
  src/factories.cpp
)
target_link_libraries(factories
  PUBLIC
    spdlog::spdlog
)
list(PREPEND ORTHODOCS_LIBS factories)

###############################################################################
# main program
###############################################################################
# cmake_print_variables(ORTHODOCS_INCS ORTHODOCS_LIBS)
configure_file(src/config.h.in ${PROJECT_SOURCE_DIR}/src/config.h @ONLY)
add_executable(orthodocs
  src/extensions.cpp
  src/main.cpp
)
target_include_directories(orthodocs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(orthodocs
  PRIVATE
    CLI11::CLI11
    ${ORTHODOCS_LIBS}
)
# cmake_print_properties(
#     TARGETS orthodocs
#     PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
# )

###############################################################################
# tests
###############################################################################
if (OPTION_OTHER_TESTS)
  add_subdirectory(tests-other)
endif()

if (OPTION_UNIT_TESTS)
  add_subdirectory(unit-tests)
endif()
