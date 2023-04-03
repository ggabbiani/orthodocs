project(commons_prj VERSION ${GIT_BRANCH_VERSION} LANGUAGES CXX)

file(TO_NATIVE_PATH "${CMAKE_INSTALL_DATADIR}"      __native_install_datadir__)
file(TO_NATIVE_PATH "${CMAKE_INSTALL_FULL_DATADIR}" __native_install_full_datadir__)
configure_file(config.h.in include/commons/config.h @ONLY)

###############################################################################
# Jason resources from github spdx
set(SPDX_LICENSES_JSON licenses.json)
file(DOWNLOAD
  https://raw.githubusercontent.com/spdx/license-list-data/main/json/licenses.json
  "${ODOX_DATA_OUTPUT_DIRECTORY}/${SPDX_LICENSES_JSON}"
)
set(SPDX_EXCEPTIONS_JSON exceptions.json)
file(DOWNLOAD
  https://raw.githubusercontent.com/spdx/license-list-data/main/json/exceptions.json
  "${ODOX_DATA_OUTPUT_DIRECTORY}/${SPDX_EXCEPTIONS_JSON}"
)

install(
  FILES
    "${ODOX_DATA_OUTPUT_DIRECTORY}/${SPDX_LICENSES_JSON}"
    "${ODOX_DATA_OUTPUT_DIRECTORY}/${SPDX_EXCEPTIONS_JSON}"
  TYPE DATA
  COMPONENT runtime
)
configure_file(spdx_config.h.in include/commons/spdx_config.h @ONLY)

# builds spdx parser
add_subdirectory(spdx)

add_library(commons STATIC
  src/dgraph.cpp
  src/document.cpp
  src/exceptions.cpp
  src/extensions.cpp
  src/globals.cpp
  src/spdx.cpp
  src/utils.cpp
)
set_target_properties(commons
  PROPERTIES
    POSITION_INDEPENDENT_CODE ${ODOX_SHARED_LIBS}
)
target_include_directories(commons
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  PRIVATE
    src
)
target_link_libraries(commons
  PUBLIC
    indicators::indicators
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    spdx::parser
  PRIVATE
    $<$<PLATFORM_ID:Windows>:utf8::cpp>
)
if (ODOX_TESTS)
  add_subdirectory(tests)
endif()
