# Release, RelWithDebInfo and MinSizeRel use embedded batch shaders.
if(CONFIG STREQUAL "Debug")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${SOURCE}" "${DESTINATION}"
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
