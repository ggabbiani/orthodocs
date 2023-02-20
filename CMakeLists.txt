cmake_minimum_required(VERSION 3.24..3.25 FATAL_ERROR)
# try to get rid of CMake Warning (dev) about policy CMP0135.
# Unfortunately it seems not able to make it less verbouse for a bug on
# cmake versions >= 3.24
cmake_policy(SET CMP0135 NEW)
# NEW behavior is to add link flags if POSITION_INDEPENDENT_CODE is set
cmake_policy(SET CMP0083 NEW)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cpack.d/cmake")

include(GitUtils)
git_branch_version(VARIABLE GIT_BRANCH_VERSION)

project(OrthoDocs
  VERSION ${GIT_BRANCH_VERSION}
  DESCRIPTION "API documentation generator and static analysis tool"
  LANGUAGES CXX
)

include(CMakeDependentOption)
include(CMakePrintHelpers)
include(CheckPIESupported)
include(CPack.d)
include(CTest)
include(FetchContent)
include(GNUInstallDirs)

option(OPTION_FUNC_TESTS      "Functional tests"                    ON  )
option(OPTION_LANGUAGE_SCAD   "OpenSCAD language extension"         ON  )
option(OPTION_OTHER_TESTS     "Other more experimental tests"       OFF )
option(OPTION_UNIT_TESTS      "Unit tests"                          ON  )
option(OPTION_WRITER_MARKDOWN "Markdown writer extension"           ON  )
option(OPTION_SCAD_VISITORS   "Implements parsing through visitors" OFF )
option(OPTION_ANTLR4_SHARED   "SHARED vs STATIC ANTLR4 runtime"     OFF )

option(OPTION_PKG             "Build of target platform packages"   OFF )

set(CMAKE_POSITION_INDEPENDENT_CODE ${OPTION_ANTLR4_SHARED})
if (CMAKE_POSITION_INDEPENDENT_CODE)
  # Check if PIE is supported on the build system
  check_pie_supported(OUTPUT_VARIABLE output LANGUAGES CXX)
  if (NOT CMAKE_CXX_LINK_PIE_SUPPORTED)
    message(AUTHOR_WARNING
      "PIE is not supported at link time:\n"
      "${output}\n"
    )
  endif()
endif()
message(STATUS "Position Independent Code ${CMAKE_POSITION_INDEPENDENT_CODE}")
# compiler must be 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# disabling GNU extension make cmake use -std=c++17 and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

# disable in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR “In-source build detected!”)
endif()

message(STATUS "Project will be installed to ${CMAKE_INSTALL_PREFIX}")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")

# static library as default in case of add_library() with no type specifier
set(BUILD_SHARED_LIBS OFF)

# override to set data in project specific directory
set(CMAKE_INSTALL_DATADIR           "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}"      )

# build directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}" )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}" )
# used during tests
set(ODOX_DATA_OUTPUT_DIRECTORY      "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}")

# Offer the user the choice of overriding the installation directories
# These variables are used during install()
set(INSTALL_DATADIR     "${CMAKE_INSTALL_DATADIR}"    CACHE PATH "Installation directory for data")
set(INSTALL_LIBDIR      "${CMAKE_INSTALL_LIBDIR}"     CACHE PATH "Installation directory for libraries")
set(INSTALL_BINDIR      "${CMAKE_INSTALL_BINDIR}"     CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDEDIR  "${CMAKE_INSTALL_INCLUDEDIR}" CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKEDIR CMake)
else()
  set(DEF_INSTALL_CMAKEDIR share/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKEDIR ${DEF_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")

# Report to user
foreach(p LIB BIN INCLUDE CMAKE DATA)
  file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path )
  message(STATUS "Installing ${p} components to ${_path}")
  unset(_path)
endforeach()

###############################################################################
# propedeutics for antrl4cpp
###############################################################################
if (NOT OPTION_ANTLR4_SHARED)
  # required if linking to antlr4 static library
  add_definitions(-DANTLR4CPP_STATIC)
endif()
# required on Windows
if (CMAKE_HOST_WIN32)
  add_definitions(-DNOGDI)
endif()

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
# set(ANTLR4_WITH_STATIC_CRT OFF)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the libary source.
#set(ANTLR4_TAG 4.11.1)
# set(ANTLR4_ZIP_REPOSITORY https://github.com/antlr/antlr4/archive/refs/tags/4.10.1.zip)
set(ANTLR4_ZIP_REPOSITORY ${PROJECT_SOURCE_DIR}/tools/antlr4-4.12.0.zip)
# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
# set(ANTLR_EXECUTABLE /home/user/antlr-4.10.1-complete.jar)
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/tools/antlr-4.12.0-complete.jar)
# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
#include_directories(${ANTLR4_INCLUDE_DIRS})
# cmake_print_variables(ANTLR4_INCLUDE_DIRS)
###############################################################################

###############################################################################
# propedeutics for orthodocs
###############################################################################

if (OPTION_ANTLR4_SHARED)
  add_library(antlr4::runtime ALIAS antlr4_shared)
else()
  add_library(antlr4::runtime ALIAS antlr4_static)
endif()

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

# We don't want json library's tests
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2
)

FetchContent_MakeAvailable(cli11 json indicators spdlog)

find_package(Boost 1.74 REQUIRED COMPONENTS unit_test_framework)
find_package(Python 3.10 COMPONENTS Interpreter)
if (NOT Python_FOUND)
  message(AUTHOR_WARNING "Python version >= 3.10 required: no version bump and unit test possible")
endif()

find_program(DIFF_EXE diff)

###############################################################################
# commons
###############################################################################
add_subdirectory(commons)

###############################################################################
# factories
###############################################################################
add_subdirectory(factories)
list(PREPEND ORTHODOCS_LIBS factories)

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
  list(APPEND ORTHODOCS_LIBS language::scad)
endif()

###############################################################################
# Markdown writer extension
###############################################################################
if (OPTION_WRITER_MARKDOWN)
  set(WRITER      Markdown)
  set(WRITER_LIB  Markdown)

  add_subdirectory(extensions/${WRITER})
  list(APPEND ORTHODOCS_LIBS writer::markdown)
endif()

###############################################################################
# main program
###############################################################################
# cmake_print_variables(ORTHODOCS_INCS ORTHODOCS_LIBS)
add_executable(orthodocs
  src/analyzer.cpp
  src/main.cpp
)
target_include_directories(orthodocs
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(orthodocs
  PRIVATE
    CLI11::CLI11
    ${ORTHODOCS_LIBS}
)
install(
  TARGETS
    orthodocs
  LIBRARY
    DESTINATION "${INSTALL_LIBDIR}"
    COMPONENT lib
  RUNTIME
    DESTINATION "${INSTALL_BINDIR}"
    COMPONENT bin
)
# cmake_print_properties(
#     TARGETS orthodocs commons spdx::parser ${ORTHODOCS_LIBS} antlr4_static antlr4_shared
#     PROPERTIES
#       POSITION_INDEPENDENT_CODE
#       IMPORTED_LOCATION IMPORTED_IMPLIB
# )
# cmake_print_variables(
#   ANTLR4_RUNTIME_LIBRARIES
#   ANTLR4_SHARED_LIBRARIES
#   ANTLR4_STATIC_LIBRARIES
#   ANTLR4_TAG
#   ORTHODOCS_LIBS
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

include(CMakeCPack.cmake)
