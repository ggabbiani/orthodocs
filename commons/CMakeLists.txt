project(commons VERSION ${GIT_BRANCH_VERSION} LANGUAGES CXX)

configure_file(config.h.in ${PROJECT_SOURCE_DIR}/include/commons/config.h @ONLY)
###############################################################################
# Jason resources from github spdx

set(SPDX_LICENSES_JSON licenses.json)
file(DOWNLOAD
  https://raw.githubusercontent.com/spdx/license-list-data/main/json/licenses.json
  "${ODOX_BUILD_DATADIR}/${SPDX_LICENSES_JSON}"
)
set(SPDX_EXCEPTIONS_JSON exceptions.json)
file(DOWNLOAD
  https://raw.githubusercontent.com/spdx/license-list-data/main/json/exceptions.json
  "${ODOX_BUILD_DATADIR}/${SPDX_EXCEPTIONS_JSON}"
)

install(
  FILES
    "${ODOX_BUILD_DATADIR}/${SPDX_LICENSES_JSON}"
    "${ODOX_BUILD_DATADIR}/${SPDX_EXCEPTIONS_JSON}"
  TYPE DATA
)
configure_file(spdx_config.h.in ${PROJECT_SOURCE_DIR}/include/commons/spdx_config.h @ONLY)

# builds spdx parser
add_subdirectory(spdx)

add_library(commons
  src/document.cpp
  src/extensions.cpp
  src/globals.cpp
  src/spdx.cpp
  src/utils.cpp
)
target_include_directories(commons
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    src
)
target_link_libraries(commons
  PUBLIC
    indicators::indicators
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    spdx::parser
)

add_subdirectory(tests)
