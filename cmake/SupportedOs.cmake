# set the following variables depending on the hosting linux system:
# OS_ID                     fedora,debian,ubuntu,darwin,win32,win64
# OS_ARCHITECTURE           hw architecture
# OS_CPACK                  cpack generator
# OS_PACKAGE_INSTALL_PREFIX installation directory for packages

function(os_package_file_name)
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

  if (UNIX)
    if (CMAKE_SYSTEM_NAME MATCHES Linux)
      distro_package_file_name(NAME ${arg_NAME} VERSION ${arg_VERSION} RELEASE ${arg_RELEASE} VARIABLE __result__)
    elseif (APPLE)
      set(__result__ "${arg_NAME}_${arg_VERSION}-${arg_RELEASE}-${OS_ID}_${OS_ARCHITECTURE}")
    else()
      message(FATAL_ERROR "Unmanaged OS ${CMAKE_HOST_SYSTEM_NAME}")
    endif()
  elseif(WIN32)
    set(__result__ "${arg_NAME}_${arg_VERSION}-${arg_RELEASE}-${OS_ID}")
  else()
    message(FATAL_ERROR "Unmanaged OS ${CMAKE_HOST_SYSTEM_NAME}")
  endif()
  set(${arg_VARIABLE} "${__result__}" PARENT_SCOPE)
endfunction(os_package_file_name)

if (UNIX)

  if (CMAKE_SYSTEM_NAME MATCHES Linux)
    include(SupportedDistro)
    set(OS_ID                     ${DISTRO_ID})
    set(OS_ARCHITECTURE           ${DISTRO_ARCHITECTURE})
    set(OS_CPACK                  ${DISTRO_CPACK})
    set(OS_PACKAGE_INSTALL_PREFIX ${DISTRO_PACKAGE_INSTALL_PREFIX})

  elseif (APPLE)
    string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}"  OS_ID)
    set(OS_ARCHITECTURE                         ${CMAKE_HOST_SYSTEM_PROCESSOR})
    set(OS_CPACK                                productbuild)
    set(OS_PACKAGE_INSTALL_PREFIX               "/opt/${PROJECT_NAME}")
  endif()

elseif (WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OS_ID "win64")
    set(OS_PACKAGE_INSTALL_PREFIX  "Program Files/${PROJECT_NAME}")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(OS_ID "win32")
    set(OS_PACKAGE_INSTALL_PREFIX  "Program Files (x86)/${PROJECT_NAME}")
  endif()
  set(OS_ARCHITECTURE "${CMAKE_HOST_SYSTEM_PROCESSOR}")
  set(OS_CPACK        "NSIS")

else()
  message(FATAL_ERROR "Unmanaged Operative System")
endif()

cmake_print_variables(
  OS_ID
  OS_ARCHITECTURE
  OS_CPACK
  OS_PACKAGE_INSTALL_PREFIX
)
