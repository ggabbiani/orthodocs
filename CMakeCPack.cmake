include(LinuxDistro)

get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
message(STATUS "component to install: ${CPACK_COMPONENTS_ALL}")

# configuration part
file(WRITE
  "${PROJECT_BINARY_DIR}/packaging/DESCRIPTION.txt"
  ${ODOX_PACKAGE_DESCRIPTION_LIST}
)
string(TOLOWER "${PROJECT_NAME}"            __lower_project_name__)
git_commits(VARIABLE                        __package_release__)
string(TOLOWER "${CMAKE_HOST_SYSTEM_NAME}"  __lower_host_system_name__)

# cross section
set(CPACK_PACKAGE_VENDOR              "Giampiero Gabbiani <giampiero@gabbiani.org>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION_MAJOR       ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR       ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH       ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION             ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE       "${PROJECT_SOURCE_DIR}/LICENSE")
if (CMAKE_BUILD_TYPE STREQUAL Release)
  set(CPACK_STRIP_FILES TRUE)
  message(STATUS "Packaged executable strip: ${CPACK_STRIP_FILES}")
else()
  message(AUTHOR_WARNING "Packaging a ${CMAKE_BUILD_TYPE} build")
endif()

# source generator
set(CPACK_SOURCE_GENERATOR            "ZIP;TGZ")
# set(CPACK_SOURCE_IGNORE_FILES         "${PROJECT_BINARY_DIR};/.git/;.gitignore;.vscode/")
set(CPACK_SOURCE_IGNORE_FILES         "${PROJECT_BINARY_DIR};.vscode/")
set(CPACK_SOURCE_PACKAGE_FILE_NAME    "${__lower_project_name__}-${CPACK_PACKAGE_VERSION}-${__package_release__}")

# UNIX
if (UNIX)
  # LINUX
  if(CMAKE_SYSTEM_NAME MATCHES Linux)

  message(STATUS "Found Linux distro id: '${DISTRO_ID}'")
    set(CPACK_PACKAGING_INSTALL_PREFIX  "/usr")
    set(CPACK_PACKAGE_DESCRIPTION_FILE  "${PROJECT_BINARY_DIR}/packaging/DESCRIPTION.txt")
    list(APPEND CPACK_GENERATOR         ${DISTRO_CPACK})
    distro_package_file_name(
      NAME      ${__lower_project_name__}
      VERSION   ${CPACK_PACKAGE_VERSION}
      RELEASE   ${__package_release__}
      VARIABLE  CPACK_PACKAGE_FILE_NAME
    )
    cmake_print_variables(
      CPACK_PACKAGE_FILE_NAME
    )

    if (DISTRO_ID STREQUAL debian OR DISTRO_ID STREQUAL ubuntu)
      set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_VENDOR}")
      set(CPACK_DEBIAN_PACKAGE_SECTION    "devel")
      set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

    elseif (DISTRO_ID STREQUAL fedora)
      set(CPACK_RPM_PACKAGE_RELEASE       ${__package_release__})
      set(CPACK_RPM_PACKAGE_LICENSE       "GPL-3.0-or-later")
      # CPACK_RPM_PACKAGE_GROUP is deprecated on Fedora
      set(CPACK_RPM_PACKAGE_GROUP         Unspecified)
      set(CPACK_RESOURCE_FILE_README      "${PROJECT_SOURCE_DIR}/README.md")

    else()
      message(AUTHOR_WARNING "Unmanaged distro id: ${DISTRO_ID}")
    endif()
  # MACOS
  elseif(APPLE)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${PROJECT_NAME}")
    set(CPACK_PACKAGE_FILE_NAME "${__lower_project_name__}-${CPACK_PACKAGE_VERSION}-${__lower_host_system_name__}")

    # post-flight.sh will add system-wide PATH env
    set(__path_entry__ "${PROJECT_BINARY_DIR}/packaging/macos/100-orthodocs")
    configure_file(packaging/macos/orthodocs-path.in "${__path_entry__}")
    configure_file(packaging/macos/post-flight.sh.in packaging/macos/post-flight.sh @ONLY)
    set(CPACK_POSTFLIGHT_RUNTIME_SCRIPT "${PROJECT_BINARY_DIR}/packaging/macos/post-flight.sh")

    configure_file(
      packaging/macos/INSTALL_NOTES.txt.in
      "${PROJECT_BINARY_DIR}/packaging/macos/INSTALL_NOTES.txt"
      @ONLY
    )
    configure_file(
      packaging/macos/Welcome.txt.in
      packaging/macos/Welcome.txt
      @ONLY
    )
    # currently only .rtfd, .rtf, .html, and .txt files are allowed on macOS.
    # to avoid a bad file extension error we rename the existing files accordingly
    configure_file(
      LICENSE
      "${PROJECT_BINARY_DIR}/packaging/macos/LICENSE.txt"
      COPYONLY
    )

    set(CPACK_RESOURCE_FILE_README    "${PROJECT_BINARY_DIR}/packaging/macos/INSTALL_NOTES.txt" )
    set(CPACK_RESOURCE_FILE_LICENSE   "${PROJECT_BINARY_DIR}/packaging/macos/LICENSE.txt"       )
    set(CPACK_RESOURCE_FILE_WELCOME   "${PROJECT_BINARY_DIR}/packaging/macos/Welcome.txt"       )
    set(CPACK_PRODUCTBUILD_IDENTIFIER "org.gabbiani.${PROJECT_NAME}"                            )

    # productbuild
    list(APPEND CPACK_GENERATOR "productbuild")
    set(CPACK_PRODUCTBUILD_IDENTIFIER "org.gabbiani.orthodocs")
  endif()
  message(STATUS "Package install prefix: '${CPACK_PACKAGING_INSTALL_PREFIX}'")

# WINDOWS
elseif(WIN32 OR MINGW)
  list(APPEND CPACK_GENERATOR                     "NSIS"                    )
  set(CPACK_NSIS_PACKAGE_NAME                     "${PROJECT_NAME}"         )
  SET(CPACK_NSIS_INSTALL_ROOT                     "C:")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    set(CPACK_SYSTEM_NAME "win64")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY           "Program Files\\\\${CPACK_NSIS_PACKAGE_NAME}")
  elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    set(CPACK_SYSTEM_NAME "win32")
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY           "Program Files (x86)\\\\${CPACK_NSIS_PACKAGE_NAME}")
  endif()
  set(CPACK_PACKAGE_FILE_NAME                     "${__lower_project_name__}-${CPACK_PACKAGE_VERSION}-${__package_release__}-${CPACK_SYSTEM_NAME}")
  set(CPACK_NSIS_MODIFY_PATH                      ON                        )
  set(CPACK_NSIS_CONTACT                          "${CPACK_PACKAGE_VENDOR}" )
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL  ON                        )
  set(CPACK_NSIS_MENU_LINKS                       "https://github.com/ggabbiani/orthodocs" "${PROJECT_NAME} Web Site")
  set(CPACK_NSIS_CONTACT                          "${CPACK_PACKAGE_VENDOR}")
  # The display name string that appears in the Windows Apps & features in Control Panel
  SET(CPACK_NSIS_DISPLAY_NAME                     "${CPACK_NSIS_PACKAGE_NAME}")
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY          "${CPACK_NSIS_PACKAGE_NAME}")

  file(TO_NATIVE_PATH "${CPACK_NSIS_INSTALL_ROOT}/${CPACK_PACKAGE_INSTALL_DIRECTORY}" __native_install_path__)
  message(STATUS "Package install prefix: '${__native_install_path__}'")
endif()

message(STATUS "CPack generators: ${CPACK_GENERATOR}")

include(CPack)
