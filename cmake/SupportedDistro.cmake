# set the following variables depending on the hosting linux system:
# DISTRO_ID                     fedora,debian,ubuntu
# DISTRO_VERSION_ID
# DISTRO_ARCHITECTURE           hw architecture
# DISTRO_CPACK                  cpack generator
# DISTRO_PACKAGE_INSTALL_PREFIX installation directory for packages

include(CMakePrintHelpers)

function(distro_tag TAG)
  set(opts)
  set(ones VARIABLE)
  set(multi)
  cmake_parse_arguments(arg "${opts}" "${ones}" "${multi}" ${ARGN})
  execute_process(
    COMMAND cat /etc/os-release
    COMMAND grep "^${TAG}=.*"
    COMMAND sed -e "s/${TAG}=//g"
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE ${arg_VARIABLE}
  )
  set(${arg_VARIABLE} "${${arg_VARIABLE}}" PARENT_SCOPE)
endfunction(distro_tag)

# returns in ${VARIABLE} the package file name WITHOUT generator extension
function(distro_package_file_name)
  set(opts)
  set(ones NAME VERSION RELEASE VARIABLE)
  set(multi)
  cmake_parse_arguments(arg "${opts}" "${ones}" "${multi}" ${ARGN})

  if (NOT arg_NAME)
    message(FATAL_ERROR "Missing NAME")
  endif()
  if (NOT arg_VERSION)
    message(FATAL_ERROR "Missing VERSION")
  endif()
  if (NOT arg_RELEASE GREATER_EQUAL 0)
    message(FATAL_ERROR "Missing RELEASE")
  endif()
  if (NOT arg_VARIABLE)
    message(FATAL_ERROR "Missing VARIABLE")
  endif()

  if (DISTRO_ID STREQUAL fedora)
    # see also [Using %{?dist}](https://docs.fedoraproject.org/en-US/packaging-guidelines/DistTag/)
    set(
      ${arg_VARIABLE}
      "${arg_NAME}-${arg_VERSION}-${arg_RELEASE}.fc${DISTRO_VERSION_ID}.${DISTRO_ARCHITECTURE}"
      PARENT_SCOPE
    )
  elseif (DISTRO_ID STREQUAL ubuntu)
    # from [PackagingVersioningScheme](https://wiki.ubuntu.com/AutoStatic/PackagingVersioningScheme)
    # Ubuntu and Debian have slightly different package versioning schemes to
    # avoid conflicting packages with the same source version. If a Debian
    # package has been changed in Ubuntu, it has ubuntuX (where X is the Ubuntu
    # revision number) appended to the end of the Debian version. So if the
    # Debian hello 2.4-1 package was changed by Ubuntu, the version string would
    # be 2.4-1ubuntu1. If a package for the application does not exist in Debian,
    # then the Debian revision is 0 (e.g., 2.4-0ubuntu1).
    if (arg_RELEASE GREATER "0")
      set(
        ${arg_VARIABLE}
        "${arg_NAME}_${arg_VERSION}-0ubuntu${arg_RELEASE}_${DISTRO_ARCHITECTURE}"
        PARENT_SCOPE
      )
    else()
      set(
        ${arg_VARIABLE}
        "${arg_NAME}_${arg_VERSION}_${DISTRO_ARCHITECTURE}"
        PARENT_SCOPE
      )
    endif()
  else()
    message(FATAL_ERROR "Unknown '${DISTRO_ID}'")
  endif()
endfunction(distro_package_file_name)

if (UNIX AND CMAKE_SYSTEM_NAME MATCHES Linux)
  set(DISTRO_PACKAGE_INSTALL_PREFIX /usr)
  if(EXISTS /etc/os-release)
    distro_tag(ID         VARIABLE DISTRO_ID)
    distro_tag(VERSION_ID VARIABLE DISTRO_VERSION_ID)
    if (DISTRO_ID STREQUAL fedora)
      set(DISTRO_ARCHITECTURE "${CMAKE_HOST_SYSTEM_PROCESSOR}")
      set(DISTRO_CPACK        "RPM")
      # cmake_print_variables(DISTRO_ID DISTRO_VERSION_ID DISTRO_ARCHITECTURE DISTRO_CPACK)
    elseif (DISTRO_ID STREQUAL ubuntu)
      if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL x86_64)
        set(DISTRO_ARCHITECTURE "amd64")
      else()
        set(DISTRO_ARCHITECTURE "${CMAKE_HOST_SYSTEM_PROCESSOR}")
      endif()
      set(DISTRO_CPACK        "DEB")
      # see [Why are Debian package file names so long?](https://www.debian.org/doc/manuals/debian-faq/pkg-basics.en.html)
      # <foo>_<VersionNumber>-<DebianRevisionNumber>_<DebianArchitecture>.deb
      # set(DISTRO_DIST         "ubuntu")
    else()
      message(FATAL_ERROR "Unknown distro id: ${DISTRO_ID}")
    endif()
  else()
    message(FATAL_ERROR "No /etc/os-release found")
  endif()
endif()

cmake_print_variables(
  DISTRO_ID
  DISTRO_ARCHITECTURE
  DISTRO_CPACK
  DISTRO_PACKAGE_INSTALL_PREFIX
)
