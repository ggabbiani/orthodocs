#
# CMake build system for OrthoDocs
#
# This file is part of the 'OrthoDocs' (ODOX) project.
#
# Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

cmake_minimum_required(VERSION 3.22..3.25 FATAL_ERROR)
# NEW behavior is to add link flags if POSITION_INDEPENDENT_CODE is set
cmake_policy(SET CMP0083 NEW)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-sdk/modules;${CMAKE_MODULE_PATH}")
include(cdk)
cdk_setup(VARIABLE GIT WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

project(OrthoDocs
  VERSION       ${GIT_BRANCH_VERSION}
  DESCRIPTION   "OpenSCAD documentation generator"
  HOMEPAGE_URL  "https://github.com/ggabbiani/orthodocs"
  LANGUAGES     CXX
)

include(CMakeDependentOption)
include(CMakePrintHelpers)
include(CheckPIESupported)
include(CTest)
include(FetchContent)
include(GNUInstallDirs)

option(ODOX_LANGUAGE_SCAD   "OpenSCAD language extension"     ON  )
option(ODOX_PKG             "Platform specific package build" ON  )
option(ODOX_SHARED_LIBS     "SHARED vs STATIC ANTLR4 runtime" OFF )
option(ODOX_WRITER_MARKDOWN "Markdown writer extension"       ON  )

option(ODOX_TESTS                       "Enable tests"            OFF )
cmake_dependent_option(ODOX_FUNC_TESTS  "Functional tests"        ON "ODOX_TESTS" OFF)
cmake_dependent_option(ODOX_OTHER_TESTS "More experimental tests" ON "ODOX_TESTS" OFF)
cmake_dependent_option(ODOX_UNIT_TESTS  "Unit tests"              ON "ODOX_TESTS" OFF)

option(ODOX_EXPERIMENTAL                  "Incomplete features"   OFF )
cmake_dependent_option(ODOX_SCAD_VISITORS "Alternative AST walkers"             OFF "ODOX_EXPERIMENTAL" OFF)
cmake_dependent_option(ODOX_DOT_GRAPH     "Alternative graph via dot/graphviz"  OFF "ODOX_EXPERIMENTAL" OFF)

###############################################################################
# This package description is formatted in lines of max 80 chars.
# It is used to produce the README.md file and to be included in RPM packages.
string(CONCAT ODOX_PACKAGE_DESCRIPTION
  "${PROJECT_NAME} is an API documentation generator and static analysis tool for the\n"
  "OpenSCAD scripting language"
)
string(REPLACE "\n" "\n;" ODOX_PACKAGE_DESCRIPTION_LIST "${ODOX_PACKAGE_DESCRIPTION}")
configure_file(README.md.in "${PROJECT_SOURCE_DIR}/README.md" @ONLY)

###############################################################################
# Position independent code (PIC) mandatory when building shared libs
set(CMAKE_POSITION_INDEPENDENT_CODE ${ODOX_SHARED_LIBS})
if (CMAKE_POSITION_INDEPENDENT_CODE)
  # Check if PIE is supported on the build system
  check_pie_supported(OUTPUT_VARIABLE output LANGUAGES CXX)
  if (NOT CMAKE_CXX_LINK_PIE_SUPPORTED)
    message(FATAL_ERROR
      "PIE is required but not supported:\n"
      "${output}\n"
    )
  endif()
endif()

###############################################################################
# C++ standard features set to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# disabling GNU extension make cmake use -std=c++17 and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

###############################################################################
# disable in-source builds
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR “In-source build detected!”)
endif()

###############################################################################
# default build type set to 'Release'
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  cdk_message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")
endif()

###############################################################################
# build directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${PROJECT_BINARY_DIR}/lib" )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  "${PROJECT_BINARY_DIR}/lib" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  "${PROJECT_BINARY_DIR}/bin" )

###############################################################################
# CMake install (per-user) default locations configuration.
# =============================================================================
# CMake decision not to provide a convenient cmake un-install feature, leads to
# problems WHEN MODIFYING THE INSTALLATION DETAILS BEFORE EXECUTING THE XARGS
# DRIVEN UN-INSTALL PROCEDURE. In such a condition subsequent cmake
# installations overwrite the build/install_manifest.txt loosing information
# about the currently installed items. To mitigate the problem CMake
# installation is configured as per-user/single directory basis.
# =============================================================================
# For the CPack install (system-wide installation) see CMakeCPack.cmake.
set_if(CMAKE_INSTALL_PREFIX
  IF_UNIX "$ENV{HOME}/.local"
  # value from Windows environment variable must be converted into a
  # cmake-style path with forward-slashes (/).
  IF_WIN TO_CMAKE_PATH "$ENV{LOCALAPPDATA}\\${CMAKE_PROJECT_NAME}"
)
set_if(CMAKE_INSTALL_CMAKEDIR
  IF_UNIX "share/cmake/${PROJECT_NAME}"
  IF_WIN  CMake
)
set(CMAKE_INSTALL_FULL_CMAKEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_CMAKEDIR}")
set(CMAKE_INSTALL_DATADIR       "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")
set(CMAKE_INSTALL_FULL_DATADIR  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
# Report to user
cdk_report("LIB;BIN;INCLUDE;CMAKE;DATA;DOC")
###############################################################################
# CMake build default locations configuration.
set(ODOX_ABS_BUILD_DATADIR      "${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_DATADIR}")
cdk_message(STATUS "Build data dir: '${ODOX_ABS_BUILD_DATADIR}'")

###############################################################################
# propaedeutic for antrl4cpp
###############################################################################
find_program(PATCH patch)

# Specify the version of the antlr4 library needed for this project.
# By default the latest version of antlr4 will be used.  You can specify a
# specific, stable version by setting a repository tag value or a link
# to a zip file containing the library source.
set(ANTLR4_TAG 4.12.0)

# prepares the structure for using the CMake macros provided
# by antlr4 Cpp runtime enriching the build directory:
#
# build
# ├── antlr4_cmake
# │   ├── antlr4-generator.cmake.in
# │   ├── Antlr4Package.md
# │   ├── antlr4-runtime.cmake.in
# │   ├── ExternalAntlr4Cpp.cmake   <=== this file must be patched for building under Windows systems
# │   ├── FindANTLR.cmake
# │   └── README.md
set(ODOX_ANTLR4_CPP_CMAKE_DIR "${CMAKE_BINARY_DIR}/antlr4_cmake")
if (NOT EXISTS "${ODOX_ANTLR4_CPP_CMAKE_DIR}")
  file(MAKE_DIRECTORY "${ODOX_ANTLR4_CPP_CMAKE_DIR}")
  execute_process(
    COMMAND
      "${CMAKE_COMMAND}" -E tar xzf "${CMAKE_SOURCE_DIR}/tools/antlr4-cpp-runtime-${ANTLR4_TAG}-cmake.tgz"
    WORKING_DIRECTORY
      "${ODOX_ANTLR4_CPP_CMAKE_DIR}"
  )
  cdk_message(STATUS "Created antlr4 Cpp source directory ${ODOX_ANTLR4_CPP_CMAKE_DIR}")
  if (WIN32)
    if (PATCH)
      execute_process(
        COMMAND
          "${PATCH}" -u -b "${ODOX_ANTLR4_CPP_CMAKE_DIR}/ExternalAntlr4Cpp.cmake" -i tools/ExternalAntlr4Cpp.patch
        WORKING_DIRECTORY
          ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND_ERROR_IS_FATAL
          ANY
      )
      cdk_message(STATUS "Patch applied to ${ODOX_ANTLR4_CPP_CMAKE_DIR}/ExternalAntlr4Cpp.cmake")
    else()
      message(WARNING "No patch command found, ANTLR4 Cpp runtime build could fail on Windows systems")
    endif()
  endif()
endif()
list(APPEND CMAKE_MODULE_PATH "${ODOX_ANTLR4_CPP_CMAKE_DIR}")

if (NOT ODOX_SHARED_LIBS)
  # required if linking to antlr4 static library
  add_definitions(-DANTLR4CPP_STATIC)
endif()
# required on Windows
if (CMAKE_HOST_WIN32)
  add_definitions(-DNOGDI)
endif()

# using /MD flag for antlr4_runtime (for Visual C++ compilers only)
set(ANTLR4_WITH_STATIC_CRT OFF)
# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})
# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
# set(ANTLR_EXECUTABLE /home/user/antlr-4.10.1-complete.jar)
set(ANTLR_EXECUTABLE ${PROJECT_SOURCE_DIR}/tools/antlr-4.12.0-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
# this is needed for all client code
find_package(ANTLR REQUIRED)
###############################################################################

###############################################################################
# propaedeutic for orthodocs
###############################################################################
if (ODOX_SHARED_LIBS)
  add_library(antlr4::runtime ALIAS antlr4_shared)
else()
  add_library(antlr4::runtime ALIAS antlr4_static)
endif()

FetchContent_Declare(utfcpp
  GIT_REPOSITORY  https://github.com/nemtrif/utfcpp.git
  GIT_TAG         79835a5fa57271f07a90ed36123e30ae9741178e # v3.2.3
)
FetchContent_Declare(cpptrace
  GIT_REPOSITORY https://ggabbiani@bitbucket.org/ggabbiani/cpp-trace.git
  GIT_TAG        v1.6.10
)
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

FetchContent_MakeAvailable(
  cli11
  json
  indicators
  spdlog
  utfcpp
)

# Solution to the wrong static lib dependency installation from:
# [cmake - Disable install for FetchContent - Stack Overflow](https://stackoverflow.com/questions/65527126/disable-install-for-fetchcontent)
FetchContent_GetProperties(cpptrace)
if(NOT cpptrace_POPULATED)
  FetchContent_Populate(cpptrace)
  add_subdirectory(${cpptrace_SOURCE_DIR} ${cpptrace_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# SHARED version for boost libraries
set(Boost_USE_STATIC_LIBS OFF)
find_package(Boost 1.71 REQUIRED COMPONENTS unit_test_framework)
find_package(Python 3.10 COMPONENTS Interpreter)
if (NOT Python_FOUND)
  message(AUTHOR_WARNING "Python version >= 3.10 required: no version bump and unit test possible")
endif()
###############################################################################
# Boost Unit Test Framework seems to introduce an ICU dependency on Windows
if (ODOX_TESTS AND WIN32)
  find_package(ICU REQUIRED COMPONENTS dt in uc)
endif()

# ODOX_DOT_EXE is used in commons/config.h.in
# TODO: add this dependecy in the cpack section too
if (ODOX_DOT_GRAPH)
  find_program(ODOX_DOT_EXE dot REQUIRED)
endif()

find_program(DIFF_EXE diff REQUIRED)

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
if (ODOX_LANGUAGE_SCAD)
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
if (ODOX_WRITER_MARKDOWN)
  set(WRITER      Markdown)
  set(WRITER_LIB  Markdown)

  add_subdirectory(extensions/${WRITER})
  list(APPEND ORTHODOCS_LIBS writer::markdown)
endif()

###############################################################################
# main program
###############################################################################
# main executable
set(ODOX orthodocs-main)
set(ODOX_ABS_BUILD_EXECUTABLE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ODOX}")

add_executable(${ODOX}
  src/analyzer.cpp
  src/main.cpp
)
# set_target_properties(${ODOX}
#   PROPERTIES
    # INSTALL_RPATH @executable_path/../Frameworks
# )
target_include_directories(${ODOX}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(${ODOX}
  PRIVATE
    CLI11::CLI11
    ${ORTHODOCS_LIBS}
)
set_target_properties(${ODOX}
  PROPERTIES
    POSITION_INDEPENDENT_CODE ${ODOX_SHARED_LIBS}
)
install(
  TARGETS
    ${ODOX}
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    COMPONENT dev
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    COMPONENT runtime
)

###############################################################################
# wrapper
include(config-wrapper.cmake)
set_if(ODOX_WRAPPER
  IF_UNIX orthodocs
  IF_WIN orthodocs.bat
)
set_if(ODOX_INSTALL_WRAPPER
  IF_UNIX packaging/unix/${ODOX_WRAPPER}
  IF_WIN  packaging/windows/${ODOX_WRAPPER}
)
set(ODOX_IN_WRAPPER           ${ODOX_INSTALL_WRAPPER}.in)
set(ODOX_ABS_IN_WRAPPER       "${PROJECT_SOURCE_DIR}/${ODOX_IN_WRAPPER}")
set(ODOX_ABS_INSTALL_WRAPPER  "${PROJECT_BINARY_DIR}/${ODOX_INSTALL_WRAPPER}")

#############################################################################
# BUILD wrapper
cmake_print_variables(ODOX_INSTALL_WRAPPER ODOX_IN_WRAPPER)
set(ODOX_ABS_BUILD_WRAPPER    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ODOX_WRAPPER}")
config_wrapper(${ODOX_IN_WRAPPER} "${ODOX_ABS_BUILD_WRAPPER}"
  MODE        "BUILD"
  DATADIR     "${ODOX_ABS_BUILD_DATADIR}"
  EXECUTABLE  "${ODOX_ABS_BUILD_EXECUTABLE}"
)
#############################################################################
# INSTALL wrapper
set(ODOX_ABS_INSTALL_EXECUTABLE "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/${ODOX}")
set(ODOX_ABS_INSTALL_DATADIR    "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}")
config_wrapper(${ODOX_IN_WRAPPER} ${ODOX_INSTALL_WRAPPER}
  MODE        "INSTALL"
  DATADIR     "${ODOX_ABS_INSTALL_DATADIR}"
  EXECUTABLE  "${ODOX_ABS_INSTALL_EXECUTABLE}"
)
install(
  FILES
    "${ODOX_ABS_INSTALL_WRAPPER}"
  TYPE
    BIN
  COMPONENT
    runtime
  PERMISSIONS
    OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(
  FILES
    README.md
  TYPE
    DOC
  COMPONENT
    runtime
)
###############################################################################
# docs
###############################################################################
install(
  FILES
    LICENSE
  TYPE
    DOC
  COMPONENT
    runtime
)
###############################################################################
# tests
###############################################################################
if (ODOX_OTHER_TESTS)
  add_subdirectory(tests-other)
endif()

if (ODOX_UNIT_TESTS)
  add_subdirectory(unit-tests)
endif()

if (ODOX_PKG)
  include(CMakeCPack.cmake)
endif()
