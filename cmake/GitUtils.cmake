include(CMakePrintHelpers)

find_package(Git REQUIRED)

###############################################################################
# git_branch_version(VARIABLE «variable name» [VERBOSE])
#
# sets GIT_BRANCH_VERSION to the value of the last git tag
###############################################################################
function(git_branch_version)
  set(options VERBOSE)
  set(oneValueArgs VARIABLE)
  set(multiValueArgs)
  cmake_parse_arguments(arg "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  if (NOT arg_VARIABLE)
    message(FATAL_ERROR "Missing output variable name")
  endif()
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --abbrev=0
    OUTPUT_VARIABLE version
    COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  # ignore leading "v" from version
  string(SUBSTRING ${version} 1 -1 version)

  # set version as a list
  string(REPLACE "." ";" version_list ${version})
  # get versioning level number
  list(LENGTH version_list length)
  if (NOT length)
    message(FATAL_ERROR "Bad version string '${version}'")
  endif()
  list(GET version_list 0 version_major)

  if (length GREATER 1)
    list(GET version_list 1 version_minor)
  else()
    set(version_minor 0)
  endif()

  if (length GREATER 2)
    list(GET version_list 2 version_patch)
  else()
    set(version_patch 0)
  endif()

  set(${arg_VARIABLE} "${version_major}.${version_minor}.${version_patch}" PARENT_SCOPE)
  if (arg_VERBOSE)
    cmake_print_variables(version_major version_minor version_patch)
  endif()
endfunction(git_branch_version)

function(git_repo_name)
  # cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH PARENT)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} config --get remote.origin.url
    OUTPUT_VARIABLE repo_name
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  cmake_path(GET repo_name STEM git_repo_name)
  set(GIT_REPO_NAME ${git_repo_name} PARENT_SCOPE)
endfunction(git_repo_name)

# sets the number of commits since last tag in the passed VARIABLE name
#
# git describe --tags returns two kind of output depending on the git repo status:
# x.y.x (in case of tagged commit)
# x.y.z-n-hhhhhhhh (n-th commit after x.y.z tag, hash hhhhhhhh)
# Returns «n» in the second case and 0 in the first.
# it is assumed that the version of the package is aligned with the last repo tag
function(git_commits)
  set(opts)
  set(ones VARIABLE)
  set(multi)
  cmake_parse_arguments(arg "${opts}" "${ones}" "${multi}" ${ARGN})
  if (NOT arg_VARIABLE)
    message(FATAL_ERROR "Missing output variable name")
  endif()
  # check if GIT_EXECUTABLE is a python2 script and change it accordingly
  if (MSYS OR MINGW)
    execute_process(
      COMMAND file ${GIT_EXECUTABLE}
      WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE git_file_type
    )
    if ("${git_file_type}" MATCHES "Python script")
      set(GIT_COMMAND python2 ${GIT_EXECUTABLE})
    endif()
  else()
    set(GIT_COMMAND ${GIT_EXECUTABLE})
  endif()
  execute_process(
    COMMAND ${GIT_COMMAND} describe --tags --long
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE VERSION_TAG
  )
  string(REGEX MATCHALL "^[^-]*-([0-9]*)-.*$" dummy ${VERSION_TAG})
  if (CMAKE_MATCH_COUNT)
    set(${arg_VARIABLE} "${CMAKE_MATCH_1}" PARENT_SCOPE)
  else()
    set(${arg_VARIABLE} 0 PARENT_SCOPE)
  endif()
endfunction(git_commits)
