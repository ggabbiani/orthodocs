project(factories VERSION ${GIT_BRANCH_VERSION} LANGUAGES CXX)

add_library(factories STATIC
  src/factories.cpp
)
set_target_properties(factories
  PROPERTIES
    POSITION_INDEPENDENT_CODE ${ODOX_SHARED_LIBS}
)
target_include_directories(factories
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(factories
  PUBLIC
    commons
)
