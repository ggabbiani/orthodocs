if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12.0")
  cmake_policy(SET CMP0074 NEW)
endif()

set(WRITER_INCS
  src
  ${PROJECT_SOURCE_DIR}/src
)
set(WRITER_LIBS indicators::indicators spdlog::spdlog)

if (OPTION_LANGUAGE_SCAD)
  list(APPEND WRITER_INCS ${PROJECT_SOURCE_DIR}/extensions/${LANGUAGE}/src)
endif()

# cmake_print_variables(WRITER_INCS WRITER_LIBS)

add_library(${WRITER_LIB} STATIC
  src/markdown/graph.cpp
  src/markdown/extension.cpp
)
target_include_directories(${WRITER_LIB}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE # library dependencies
    ${WRITER_INCS}
    ${Boost_INCLUDE_DIRS})
target_link_libraries(${WRITER_LIB}
  PUBLIC # clients and library dependencies
    commons
)
