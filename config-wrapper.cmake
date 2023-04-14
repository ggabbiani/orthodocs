include(CMakePrintHelpers)

# cmake for wrapper
function(config_wrapper IN OUT)
  set(flags)
  set(opts DATADIR MODE EXECUTABLE)
  set(multi)
  cmake_parse_arguments(arg "${flags}" "${opts}" "${multi}" ${ARGN})
  if (arg_UNPARSED_ARGUMENTS OR NOT IN OR NOT OUT)
    message(FATAL_ERROR "Parameter(s) missing")
  endif()
  file(TO_NATIVE_PATH "${arg_DATADIR}"      __abs_datadir__)
  file(TO_NATIVE_PATH "${arg_EXECUTABLE}"   __abs_exe__)
  set(__mode__        "${arg_MODE}")
  configure_file(${IN} ${OUT}
    FILE_PERMISSIONS OWNER_EXECUTE OWNER_READ OWNER_WRITE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    @ONLY
  )
  cmake_print_variables(__mode__ __abs_datadir__ __abs_exe__ )
endfunction(config_wrapper)
