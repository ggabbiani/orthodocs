if (APPLE)
  # productbuild doesn't use CPACK_PACKAGE_INSTALL_DIRECTORY
  # so we leverage CPACK_PACKAGING_INSTALL_PREFIX only
  set(CPACK_PACKAGING_INSTALL_PREFIX "/Applications/${PROJECT_NAME}")
elseif(UNIX)
  set(CPACK_PACKAGING_INSTALL_PREFIX "/opt")
endif()

set(PACKAGE_CONTACT "Giampiero Gabbiani (giampiero@gabbiani.org)")

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "${PACKAGE_CONTACT}")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/INSTALL.md")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_BINARY_DIR};/.git/;.gitignore;.vscode/")
set(CPACK_SOURCE_GENERATOR "ZIP;TGZ")
set(CPACK_GENERATOR "ZIP;TGZ")

if(UNIX)
  if(CMAKE_SYSTEM_NAME MATCHES Linux)
    list(APPEND CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${PACKAGE_CONTACT}")
    set(CPACK_DEBIAN_PACKAGE_SECTION "devel")

    list(APPEND CPACK_GENERATOR "RPM")
    git_commits(VARIABLE CPACK_RPM_PACKAGE_RELEASE)
    # cmake_print_variables(CPACK_RPM_PACKAGE_RELEASE)
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0-or-later")
  endif()
endif()

if(WIN32 OR MINGW)
  list(APPEND CPACK_GENERATOR "NSIS")
  set(CPACK_NSIS_PACKAGE_NAME "message")
  set(CPACK_NSIS_CONTACT "${PACKAGE_CONTACT}")
  set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
endif()

if(APPLE)
  set(CPACK_GENERATOR "productbuild")

  # currently only .rtfd, .rtf, .html, and .txt files are allowed on macOS.
  # to avoid a bad file extension error we rename the existing files accordingly
  configure_file(packaging/macos/Welcome.txt.in Welcome.txt @ONLY)

  set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_BINARY_DIR}/Welcome.txt")
  set(CPACK_RESOURCE_FILE_README  "${PROJECT_SOURCE_DIR}/packaging/macos/README.txt")
  set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/packaging/macos/License.txt")
  set(CPACK_PRODUCTBUILD_IDENTIFIER "org.gabbiani.${PROJECT_NAME}")

  message(STATUS "PACKAGE INSTALL: ${CPACK_PACKAGING_INSTALL_PREFIX}/${CPACK_PACKAGE_INSTALL_DIRECTORY}")
  # Report to user
  foreach(p LIB BIN INCLUDE CMAKE DATA)
    file(TO_NATIVE_PATH "${CPACK_PACKAGING_INSTALL_PREFIX}/${CMAKE_INSTALL_${p}DIR}" _path )
    message(STATUS "Packaging ${p} components to ${_path}")
    unset(_path)
  endforeach()

  #list(APPEND CPACK_GENERATOR "Bundle")
  # set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
  # set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE ON)
  # configure_file(${PROJECT_SOURCE_DIR}/packaging/macos/Info.plist.in Info.plist @ONLY)
  # set(CPACK_BUNDLE_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
  # set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/packaging/macos/coffee.icns)
endif()

message(STATUS "CPack generators: ${CPACK_GENERATOR}")

include(CPack)
