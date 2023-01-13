project(factories VERSION ${GIT_BRANCH_VERSION} LANGUAGES CXX)

add_library(factories
  src/factories.cpp
)
target_include_directories(factories
  PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(factories
  PUBLIC
    commons
)
