if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.12.0")
  cmake_policy(SET CMP0074 NEW)
endif()

set(WRITER_INCS
  src
  ${PROJECT_SOURCE_DIR}/src
)
set(WRITER_LIBS indicators::indicators spdlog::spdlog)

if (ODOX_LANGUAGE_SCAD)
  list(APPEND WRITER_INCS ${PROJECT_SOURCE_DIR}/extensions/${LANGUAGE}/include)
endif()

# cmake_print_variables(WRITER_INCS WRITER_LIBS)

add_library(${WRITER_LIB} STATIC
  src/markdown/graph.cpp
  src/markdown/extension.cpp
)
set_target_properties(${WRITER_LIB}
  PROPERTIES
    POSITION_INDEPENDENT_CODE ${ODOX_SHARED_LIBS}
)
add_library(writer::markdown ALIAS ${WRITER_LIB})
target_include_directories(${WRITER_LIB}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE # library dependencies
    ${WRITER_INCS}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(${WRITER_LIB}
  PUBLIC # clients and library dependencies
    commons
)
