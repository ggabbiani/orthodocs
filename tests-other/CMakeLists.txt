if (ODOX_LANGUAGE_SCAD)
  add_executable(licenses licenses.cpp)
  target_include_directories(licenses PRIVATE ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(licenses PRIVATE commons spdlog::spdlog ${LANGUAGE_LIB})

  add_executable(xref xref.cpp)
  target_include_directories(xref PUBLIC ${PROJECT_SOURCE_DIR}/src)
  target_link_libraries(xref PRIVATE commons spdlog::spdlog ${LANGUAGE_LIB})

endif()