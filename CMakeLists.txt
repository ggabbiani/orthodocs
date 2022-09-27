# minimum required CMAKE version
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(OrthoDocs)

option(BUILD_TESTING    "Generate tests"              ON)
option(LANGUAGE_SCAD    "OpenSCAD language extension" ON)
option(WRITER_MARKDOWN  "Markdown writer extension"   ON)
option(EXPERIMENTAL     "Experimental tests"         OFF)

include(CTest)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)
# disabling GNU extension make cmake use -std=c++17 and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)

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
set(ANTLR4_ZIP_REPOSITORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr4-4.10.1.zip)

# add external build for antlrcpp
include(ExternalAntlr4Cpp)
# add antrl4cpp artifacts to project environment
include_directories(${ANTLR4_INCLUDE_DIRS})
message("ANTLR4_INCLUDE_DIRS: ${ANTLR4_INCLUDE_DIRS}")

# set variable pointing to the antlr tool that supports C++
# this is not required if the jar file can be found under PATH environment
# set(ANTLR_EXECUTABLE /home/user/antlr-4.10.1-complete.jar)
set(ANTLR_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR}/tools/antlr-4.10.1-complete.jar)
# add macros to generate ANTLR Cpp code from grammar
find_package(ANTLR REQUIRED)

configure_file(include/orthodocs/config.h.in ${PROJECT_SOURCE_DIR}/include/orthodocs/config.h @ONLY)
set(ORTHODOCS_INCS include third-part)
set(ORTHODOCS_LIBS antlr4_static)

if (LANGUAGE_SCAD)
  add_subdirectory(extensions/scad)
  # list(APPEND ORTHODOCS_INCS extensions/scad/include)
  list(APPEND ORTHODOCS_LIBS scad)
endif()

if (WRITER_MARKDOWN)
  add_subdirectory(extensions/markdown)
  list(APPEND ORTHODOCS_LIBS markdown)
endif()
message("ORTHODOCS_INCS: ${ORTHODOCS_INCS}")
message("ORTHODOCS_LIBS: ${ORTHODOCS_LIBS}")

add_executable(orthodocs
  src/orthodocs/analizer.cpp
  src/orthodocs/document.cpp
  src/orthodocs/extensions.cpp
  src/orthodocs/globals.cpp
  src/orthodocs/main.cpp
  src/orthodocs/utils.cpp
  src/orthodocs/xref.cpp
)
if (WRITER_MARKDOWN)
  target_link_options(orthodocs PUBLIC "-Wl,--undefined,markdown_extension_loader")
endif()
if (LANGUAGE_SCAD)
  target_link_options(orthodocs PUBLIC "-Wl,--undefined,scad_extension_loader")
endif()
target_include_directories(orthodocs PUBLIC ${ORTHODOCS_INCS})
target_link_libraries(orthodocs ${ORTHODOCS_LIBS})

if (EXPERIMENTAL)
  add_subdirectory(experimental)
endif()

if (BUILD_TESTING)
  add_subdirectory(tests)
endif()