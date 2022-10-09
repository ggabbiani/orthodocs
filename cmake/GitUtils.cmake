# sets GIT_BRANCH_VERSION to the value of the last git tag
function(git_branch_version)
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

  set(GIT_BRANCH_VERSION "${version_major}.${version_minor}.${version_patch}" PARENT_SCOPE)
endfunction(git_branch_version)

find_package(Git REQUIRED)
