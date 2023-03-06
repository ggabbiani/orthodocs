get_cmake_property(CPACK_COMPONENTS_ALL COMPONENTS)
message(STATUS "component to install: ${CPACK_COMPONENTS_ALL}")

set(PACKAGE_CONTACT "Giampiero Gabbiani (giampiero@gabbiani.org)")
configure_file(
  packaging/macos/Welcome.txt.in
  "${PROJECT_BINARY_DIR}/packaging/macos/Welcome.txt"
  @ONLY
)

file(WRITE
  "${PROJECT_BINARY_DIR}/packaging/DESCRIPTION.txt"
  ${ODOX_PACKAGE_DESCRIPTION_LIST}
)

# set(CPACK_PACKAGE_NAME                "${PROJECT_NAME}")
string(TOLOWER "${PROJECT_NAME}" CPACK_PACKAGE_NAME)
set(CPACK_PACKAGE_VENDOR              "${PACKAGE_CONTACT}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_PACKAGE_VERSION_MAJOR       "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR       "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH       "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION             ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
set(CPACK_SOURCE_IGNORE_FILES         "${PROJECT_BINARY_DIR};/.git/;.gitignore;.vscode/")
if (CMAKE_BUILD_TYPE STREQUAL Release)
  set(CPACK_STRIP_FILES TRUE)
  message(STATUS "Executable strip: ${CPACK_STRIP_FILES}")
endif()
# set(CPACK_SOURCE_GENERATOR "")
# set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
# set(CPACK_GENERATOR "ZIP;TGZ")

#############################################################################
# UNIX
#############################################################################
if (UNIX)
  # set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/${PROJECT_NAME}")
  #############################################################################
  # LINUX
  #############################################################################
  if(CMAKE_SYSTEM_NAME MATCHES Linux)
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

    list(APPEND CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_CONTACT}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

    list(APPEND CPACK_GENERATOR "RPM")
    git_commits(VARIABLE CPACK_RPM_PACKAGE_RELEASE)
    # cmake_print_variables(CPACK_RPM_PACKAGE_RELEASE)

    string(TOLOWER "${PROJECT_NAME}" CPACK_PACKAGE_FILE_NAME)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CMAKE_HOST_SYSTEM_PROCESSOR}")

    set(CPACK_RPM_PACKAGE_LICENSE       "GPL-3.0-or-later")
    set(CPACK_RPM_PACKAGE_GROUP         Unspecified)  # CPACK_RPM_PACKAGE_GROUP is deprecated on Fedora
    set(CPACK_PACKAGE_DESCRIPTION_FILE  "${PROJECT_BINARY_DIR}/packaging/DESCRIPTION.txt")
    set(CPACK_RESOURCE_FILE_README      "${PROJECT_SOURCE_DIR}/README.md")
  #############################################################################
  # MACOS
  #############################################################################
  elseif(APPLE)
    # currently only .rtfd, .rtf, .html, and .txt files are allowed on macOS.
    # to avoid a bad file extension error we rename the existing files accordingly
    configure_file(
      LICENSE
      "${PROJECT_BINARY_DIR}/packaging/macos/LICENSE.txt"
      COPYONLY
    )

    set(CPACK_RESOURCE_FILE_README    "${PROJECT_BINARY_DIR}/packaging/DESCRIPTION.txt")
    set(CPACK_RESOURCE_FILE_LICENSE   "${PROJECT_BINARY_DIR}/packaging/macos/LICENSE.txt")
    set(CPACK_RESOURCE_FILE_WELCOME   "${PROJECT_BINARY_DIR}/packaging/macos/Welcome.txt")
    set(CPACK_PRODUCTBUILD_IDENTIFIER "org.gabbiani.${PROJECT_NAME}")

    #############################################################################
    # productbuild
    #############################################################################
    list(APPEND CPACK_GENERATOR "productbuild")

    #############################################################################
    # DragNDrop
    #############################################################################
    # list(APPEND CPACK_GENERATOR "DragNDrop")

    #############################################################################
    # Bundle
    #############################################################################
    #list(APPEND CPACK_GENERATOR "Bundle")
    # set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
    # set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE ON)
    # configure_file(${PROJECT_SOURCE_DIR}/packaging/macos/Info.plist.in Info.plist @ONLY)
    # set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
    # set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/packaging/macos/coffee.icns)
  endif()
#############################################################################
# WINDOWS
#############################################################################
elseif(WIN32 OR MINGW)
  list(APPEND CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_PACKAGE_NAME "message")
  set(CPACK_NSIS_CONTACT "${PACKAGE_CONTACT}")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
endif()

message(STATUS "Package install prefix: ${CPACK_PACKAGING_INSTALL_PREFIX}/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
message(STATUS "CPack generators: ${CPACK_GENERATOR}")

include(CPack)
