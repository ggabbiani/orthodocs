set(WRITER markdown)
set(WRITER_INCS 
  include
  ${PROJECT_SOURCE_DIR}/include 
  ${PROJECT_SOURCE_DIR}/third-part
)

if (LANGUAGE_SCAD)
  list(APPEND WRITER_INCS ${PROJECT_SOURCE_DIR}/extensions/scad/include)
  list(APPEND WRITER_LIBS scad)
endif()

message("WRITER_INCS: ${WRITER_INCS}")
message("WRITER_LIBS: ${WRITER_LIBS}")

add_library(${WRITER} STATIC
  src/markdown/graph.cpp 
  src/markdown/extension.cpp
)
# target_link_options(${WRITER} PUBLIC "-Wl,--undefined,markdown_loader_dimmerda")
target_include_directories(${WRITER} PUBLIC ${WRITER_INCS})
target_link_libraries(${WRITER} ${WRITER_LIBS})
