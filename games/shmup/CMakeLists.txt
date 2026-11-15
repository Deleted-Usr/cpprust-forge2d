add_executable(
    shoot-em-up

    src/main.cpp
    src/ShmupGame.cpp

    src/Player.cpp
    src/Bullet.cpp
    src/Enemy.cpp
)

target_compile_features(
    shoot-em-up
    PRIVATE
        cxx_std_20
)

target_link_libraries(
    shoot-em-up

    PRIVATE
        forge2d
)

# ------------------------------------------------------------
# Compiler warnings
# ------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        shoot-em-up

        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
elseif(MSVC)
    target_compile_options(
        shoot-em-up

        PRIVATE
        /W4
    )
endif()

# ------------------------------------------------------------
# Copy game assets beside the executable
# ------------------------------------------------------------

add_custom_command(
    TARGET shoot-em-up
    POST_BUILD

    COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:shoot-em-up>/assets"

    COMMENT
    "Copying shoot-em-up assets"
)

# ------------------------------------------------------------
# Copy engine shaders beside the executable
# ------------------------------------------------------------

add_custom_command(
    TARGET shoot-em-up
    POST_BUILD

    COMMAND
    ${CMAKE_COMMAND}
    "-DCONFIG=$<CONFIG>"
    "-DSOURCE=${PROJECT_SOURCE_DIR}/engine/shaders"
    "-DDESTINATION=$<TARGET_FILE_DIR:shoot-em-up>/shaders"
    -P "${PROJECT_SOURCE_DIR}/cmake/copy-debug-shaders.cmake"

    COMMENT
    "Copying Forge2D shaders for Debug"
)
