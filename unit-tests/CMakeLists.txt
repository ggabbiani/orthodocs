# configure_file(ut_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/ut_config.h @ONLY)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)

function (test_unit) 
  set(options )
  set(oneValueArgs NAME)
  set(multiValueArgs INCLUDES LIBS SOURCES)
  cmake_parse_arguments(UNIT_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT DEFINED UNIT_TEST_NAME)
    message(FATAL_ERROR undefined test name)
  endif()

  if (NOT DEFINED UNIT_TEST_SOURCES)
    message(FATAL_ERROR undefined test sources)
  endif()

  # cmake_print_variables(UNIT_TEST_LIBS)

  string(TOLOWER "ut-${UNIT_TEST_NAME}" UNIT_TEST)
  add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES})
  if (DEFINED UNIT_TEST_INCLUDES)
    target_include_directories(${UNIT_TEST} PRIVATE ${UNIT_TEST_INCLUDES})
  endif()
  set_target_properties(${UNIT_TEST} 
    PROPERTIES 
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK 
  )
  target_link_libraries(${UNIT_TEST}
    Boost::unit_test_framework
    ${UNIT_TEST_LIBS}
  )
  add_test(NAME ${UNIT_TEST_NAME} COMMAND ${UNIT_TEST})
  # set_tests_properties(
  # Baluba Baluba1 etc etc ...
  # PROPERTIES
  # ENVIRONMENT "MD_CONFIG=${PROJECT_SOURCE_DIR}/etc/md.conf;BOOST_TEST_LOG_LEVEL=message;TEST_DATA_PATH=${PROJECT_SOURCE_DIR}/data;LD_LIBRARY_PATH=${MD_SOPATH}"
  # )
endfunction(test_unit)

# cmake_print_properties(
#     TARGETS orthodocs commons CppSCAD Markdown factories
#     PROPERTIES INTERFACE_LINK_LIBRARIES INTERFACE_INCLUDE_DIRECTORIES
# )
# cmake_print_variables(ORTHODOCS_LIBS)
test_unit(NAME UnitTests
  SOURCES 
    ut-tests.cpp 
  LIBS 
    ${ORTHODOCS_LIBS}
)
