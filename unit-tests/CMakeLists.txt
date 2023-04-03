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
      LINK_OPTIONS -fstack-protector
  )
  # if (ICU_ROOT)
  #   cmake_print_variables(ICU_ROOT)
  #   target_link_directories(${UNIT_TEST} BEFORE
  #     PUBLIC
  #       ${ICU_ROOT}/lib
  #   )
  # endif()
  target_link_libraries(${UNIT_TEST}
    PRIVATE
      Boost::unit_test_framework
      ${UNIT_TEST_LIBS}
  )
  add_test(NAME ${UNIT_TEST_NAME} COMMAND ${UNIT_TEST})
  set_tests_properties(${UNIT_TEST_NAME}
    PROPERTIES
      LABELS "UNIT_TEST"
  )
  # set_tests_properties(
  # Baluba Baluba1 etc etc ...
  # PROPERTIES
  # ENVIRONMENT "MD_CONFIG=${PROJECT_SOURCE_DIR}/etc/md.conf;BOOST_TEST_LOG_LEVEL=message;TEST_DATA_PATH=${PROJECT_SOURCE_DIR}/data;LD_LIBRARY_PATH=${MD_SOPATH}"
  # )
endfunction(boost_test)

# add a doc compare test using batch_test()
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
  set(oneValueArgs NAME SOURCES COMPARE OPTIONS SERIAL)
  set(multiValueArgs)
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
  batch_test(
    NAME ${DOC_NAME}
    SERIAL ${DOC_SERIAL}
    CMD1 "$<TARGET_FILE:${ODOX}> --data-dir \"${ODOX_DATA_OUTPUT_DIRECTORY}\" --src-root ${DOC_SOURCES} --doc-root ${DOC_ROOT} --verbosity off ${DOC_OPTIONS}"
    CMD2 "$<IF:$<BOOL:${CMAKE_HOST_UNIX}>,${DIFF_EXE},\"${DIFF_EXE}\"> ${DOC_COMPARE} ${DOC_ROOT}"
  )
  # set_property(TEST ${DOC_NAME} PROPERTY LABELS DOCUMENT DIFF)
  set_tests_properties(${DOC_NAME}
    PROPERTIES LABELS "DOCUMENT;DIFF;Python"
  )
endfunction(doc_test)

# add a doc compare test using batch.py
#
# NAME    - test name used for composing the generated document root
# COMMANDS - one string for each command
# SERIAL  - boolean indicating if the test will be performed serially or not.
#           The default is SERIAL TRUE.
function (batch_test)
  set(options)
  set(oneValueArgs NAME SERIAL CMD1 CMD2)
  set(multiValueArgs)
  cmake_parse_arguments(BATCH "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT Python_FOUND)
    message(FATAL_ERROR "Python is needed for batch_test")
  endif()
  if (NOT DEFINED BATCH_SERIAL)
    set(BATCH_SERIAL TRUE)
  endif()
  set(BATCH_EXE "${PROJECT_SOURCE_DIR}/bin/batch.py")
  # cmake_print_variables(XDIFF_NAME XDIFF_EXE XDIFF_LEFT XDIFF_RIGHT XDIFF_SOURCE_ROOT)
  add_test(
    NAME ${BATCH_NAME}
    COMMAND "${Python_EXECUTABLE}" "${BATCH_EXE}"
      "$<$<BOOL:${BATCH_CMD1}>:${BATCH_CMD1}>"
      "$<$<BOOL:${BATCH_CMD2}>:${BATCH_CMD2}>"
    )
  set_tests_properties(${BATCH_NAME}
    PROPERTIES RUN_SERIAL ${BATCH_SERIAL}
  )
endfunction(batch_test)

boost_test(NAME UnitTests
  SOURCES
    ut-tests.cpp
  LIBS
    ${ORTHODOCS_LIBS}
    ${Boost_LIBRARIES}
#    ${ICU_IN_LIBRARIES} ${ICU_DT_LIBRARIES} ${ICU_UC_LIBRARIES}
#    $<$<PLATFORM_ID:Windows>:ICU::in ICU::dt ICU::uc>
)
#cmake_print_variables(Boost_LIBRARIES ICU_IN_LIBRARIES ICU_DT_LIBRARIES ICU_UC_LIBRARIES)

cmake_print_variables(
  ODOX_DATA_OUTPUT_DIRECTORY
)

if (Python_FOUND)
  doc_test(NAME ortho-lib-defdecorations
    SOURCES docu-styles/ortho/lib-defdecorations
    COMPARE docu-styles/results/annotated
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --graphs ."
  )
  doc_test(NAME ortho-lib-custom-decorations
    SOURCES docu-styles/ortho/lib-custom-decorations
    COMPARE docu-styles/results/annotated
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --decorations \"@\" --graphs ."
  )
  doc_test(NAME ortho-lib-nocomments
    SOURCES docu-styles/ortho/lib-nocomments
    COMPARE docu-styles/results/no-annotations
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --graphs ."
  )
  doc_test(NAME ortho-lib-nodecorations
    SOURCES docu-styles/ortho/lib-nodecorations
    COMPARE docu-styles/results/annotated
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --decorations \"\" --graphs ."
  )

  doc_test(NAME unortho-lib-defdecorations
    SOURCES docu-styles/unortho/lib-defdecorations
    COMPARE docu-styles/results/annotated
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --unorthodox --graphs ."
  )
  doc_test(NAME unortho-lib-custom-decorations
    SOURCES docu-styles/unortho/lib-custom-decorations
    COMPARE docu-styles/results/annotated
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --unorthodox --decorations \"@\" --graphs ."
  )
  doc_test(NAME unortho-lib-nocomments
    SOURCES docu-styles/unortho/lib-nocomments
    COMPARE docu-styles/results/no-annotations
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --unorthodox --graphs ."
  )
  doc_test(NAME unortho-lib-nodecorations
    SOURCES docu-styles/unortho/lib-nodecorations
    COMPARE docu-styles/results/annotated
    OPTIONS "--toc --pkg-deps graph --ignore-prefix $<IF:$<BOOL:${CMAKE_HOST_UNIX}>,'\$',$> --unorthodox --decorations \"\" --graphs ."
  )
  endif()
