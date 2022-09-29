find_package(Boost REQUIRED)
message("Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}")

set(WRITER_INCS
  include
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/third-part
)

if (OPTION_LANGUAGE_SCAD)
  list(APPEND WRITER_INCS ${PROJECT_SOURCE_DIR}/extensions/${LANGUAGE}/include)
endif()

message("WRITER_INCS: ${WRITER_INCS}")
message("WRITER_LIBS: ${WRITER_LIBS}")

add_library(${WRITER_LIB} STATIC
  src/markdown/graph.cpp
  src/markdown/extension.cpp
)
target_include_directories(${WRITER_LIB}
  PUBLIC
    ${WRITER_INCS}
    ${Boost_INCLUDE_DIRS})
target_link_libraries(${WRITER_LIB} ${WRITER_LIBS})
