add_executable(
    forge2d-sandbox

    main.cpp
)

target_compile_features(
    forge2d-sandbox
    PRIVATE
        cxx_std_20
)

target_link_libraries(
    forge2d-sandbox

    PRIVATE
        forge2d
)

add_custom_command(
    TARGET forge2d-sandbox
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND}
        "-DCONFIG=$<CONFIG>"
        "-DSOURCE=${PROJECT_SOURCE_DIR}/engine/shaders"
        "-DDESTINATION=$<TARGET_FILE_DIR:forge2d-sandbox>/shaders"
        -P "${PROJECT_SOURCE_DIR}/cmake/copy-debug-shaders.cmake"
)
