# minimum required CMAKE version
cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(OrthoDocs)

option(OPTION_LANGUAGE_SCAD   "OpenSCAD language extension" ON  )
option(OPTION_WRITER_MARKDOWN "Markdown writer extension"   ON  )
option(OPTION_EXPERIMENTAL    "Experimental tests"          OFF )

include(CTest)

# compiler must be 17
set(CMAKE_CXX_STANDARD 17)
# disabling GNU extension make cmake use -std=c++17 and not -std=gnu++17
set(CMAKE_CXX_EXTENSIONS OFF)
# if (APPLE)
#   SET(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
#   SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
#   SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
#   SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
# endif()

###############################################################################
# global propedeutics to antrl4cpp
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
message("ANTLR4_INCLUDE_DIRS: ${ANTLR4_INCLUDE_DIRS}")
###############################################################################

configure_file(include/orthodocs/config.h.in ${PROJECT_SOURCE_DIR}/include/orthodocs/config.h @ONLY)
set(ORTHODOCS_INCS include third-part)
set(ORTHODOCS_LIBS antlr4_static)

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
  set(WRITER Markdown)
  set(WRITER_LIB Markdown)

  add_subdirectory(extensions/${WRITER})
  list(APPEND ORTHODOCS_LIBS ${WRITER_LIB})
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
target_include_directories(orthodocs PUBLIC ${ORTHODOCS_INCS})
target_link_libraries(orthodocs ${ORTHODOCS_LIBS})

if (OPTION_EXPERIMENTAL)
  add_subdirectory(experimental)
endif()

if (BUILD_TESTING)
  add_subdirectory(tests)
endif()