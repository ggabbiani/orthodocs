project(commons VERSION ${GIT_BRANCH_VERSION} LANGUAGES CXX)

configure_file(config.h.in ${PROJECT_SOURCE_DIR}/include/config.h @ONLY)
add_library(commons
  src/document.cpp
  src/extensions.cpp
  src/globals.cpp
  src/utils.cpp
)
target_include_directories(commons 
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(commons
  spdlog::spdlog
  indicators::indicators
)
# cmake_print_properties(
#     TARGETS orthodocs
#     PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
# )
