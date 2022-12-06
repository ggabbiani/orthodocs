# configure_file(ut_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/ut_config.h @ONLY)
# set(CMAKE_INCLUDE_CURRENT_DIR ON)


# add a boost unit test
# boost_test(NAME=<unit test name> SOURCES <source file list>  INCLUDES <include dir list> LIBS <linked lib list>)
function (boost_test) 
  set(options)
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
endfunction(boost_test)

# add a diff test
function(xdiff_test)
  set(options SERIAL)
  set(oneValueArgs NAME LEFT RIGHT)
  set(multiValueArgs CMD)
  cmake_parse_arguments(XDIFF "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  # cmake_parse_arguments(XDIFF "${options}" "${oneValueArgs}" "${multiValueArgs}")

  set(XDIFF_EXE "${PROJECT_SOURCE_DIR}/bin/xdiff.py")
  # cmake_print_variables(XDIFF_NAME XDIFF_EXE XDIFF_LEFT XDIFF_RIGHT XDIFF_SOURCE_ROOT)
  add_test(
    NAME ${XDIFF_NAME}
    COMMAND "${XDIFF_EXE}" ++left "${XDIFF_LEFT}" ++right "${XDIFF_RIGHT}" ${XDIFF_CMD}
  )
  if (NOT DEFINED XDIFF_SERIAL)
    set(XDIFF_SERIAL TRUE)
  endif()
  set_tests_properties(${XDIFF_NAME} PROPERTIES RUN_SERIAL ${XDIFF_SERIAL})
  endfunction(xdiff_test)

# add a doc compare test using xdiff_test()
#
# NAME    - test name used for composing the generated document root
# SOURCES - source directory, if not absolute is meant as current source 
#           relative
# COMPARE - document directory to be compared with, if not absolute is meant as
#           current source relative
# OPTIONS - orthodocs binary options. Note that by default the binary is always
#           called with the following parameters:
#           --src-root "${DOC_SOURCES}" 
#           --doc-root "${DOC_ROOT}"
#           --verbosity off
# SERIAL  - boolean indicating if the test will be performed serially or not.
#           The default is SERIAL TRUE.
function (doc_test)
  set(options)
  set(oneValueArgs NAME SOURCES COMPARE SERIAL)
  set(multiValueArgs OPTIONS)
  cmake_parse_arguments(DOC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  cmake_path(IS_RELATIVE DOC_SOURCES SRC_IS_RELATIVE)
  if (SRC_IS_RELATIVE)
    set(DOC_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${DOC_SOURCES})
  endif()

  cmake_path(IS_RELATIVE DOC_COMPARE CMP_IS_RELATIVE)
  if (CMP_IS_RELATIVE)
    set(DOC_COMPARE ${CMAKE_CURRENT_SOURCE_DIR}/${DOC_COMPARE})
  endif()

  if (NOT DEFINED DOC_SERIAL)
    set(DOC_SERIAL TRUE)
  endif()

  set(DOC_ROOT "${CMAKE_CURRENT_BINARY_DIR}/${DOC_NAME}")
  xdiff_test(NAME ${DOC_NAME}
    SERIAL  ${DOC_SERIAL}
    LEFT    "${DOC_COMPARE}"
    RIGHT   "${DOC_ROOT}"
    CMD     $<TARGET_FILE:orthodocs> 
      --src-root "${DOC_SOURCES}" 
      --doc-root "${DOC_ROOT}"
      --verbosity off
      ${DOC_OPTIONS}
  )
endfunction(doc_test)

# cmake_print_properties(
#     TARGETS orthodocs commons CppSCAD Markdown factories
#     PROPERTIES INTERFACE_LINK_LIBRARIES INTERFACE_INCLUDE_DIRECTORIES
# )
# cmake_print_variables(ORTHODOCS_LIBS)
boost_test(NAME UnitTests
  SOURCES 
    ut-tests.cpp 
  LIBS 
    ${ORTHODOCS_LIBS}
)

doc_test(NAME xdiff-flat-lib
  SOURCES "flat-lib/sources"
  COMPARE "flat-lib/docs"
  OPTIONS --toc --pkg-deps graph
)

doc_test(NAME xdiff-flat-lib-ignore
  SOURCES "flat-lib/sources"
  COMPARE "flat-lib/docs-ignore"
  OPTIONS --toc --pkg-deps graph --ignore-prefix prefix_ $prefix_
)
