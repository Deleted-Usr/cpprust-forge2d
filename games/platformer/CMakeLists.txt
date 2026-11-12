add_executable(
    platformer

    src/main.cpp
    src/PlatformerGame.cpp
    src/PlatformerGame.hpp
    src/assets/Assets.hpp
    src/assets/Assets.cpp

    # EXAMPLES
    # src/Platformer.cpp
    # src/Player.cpp
    # src/Enemy.cpp
    # src/Weapon.cpp
)

target_compile_features(
    platformer
    PRIVATE
        cxx_std_20
)

target_link_libraries(
    platformer

    PRIVATE
        forge2d
)

# ------------------------------------------------------------
# Compiler warnings
# ------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        platformer

        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
elseif(MSVC)
    target_compile_options(
        platformer

        PRIVATE
            /W4
    )
endif()

# ------------------------------------------------------------
# Copy game assets beside the executable
# ------------------------------------------------------------

add_custom_command(
    TARGET platformer
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:platformer>/assets"

    COMMENT
        "Copying platformer assets"
)

# ------------------------------------------------------------
# Copy engine shaders beside the executable
# ------------------------------------------------------------

add_custom_command(
    TARGET platformer
    POST_BUILD

    COMMAND
        ${CMAKE_COMMAND}
        "-DCONFIG=$<CONFIG>"
        "-DSOURCE=${PROJECT_SOURCE_DIR}/engine/shaders"
        "-DDESTINATION=$<TARGET_FILE_DIR:platformer>/shaders"
        -P "${PROJECT_SOURCE_DIR}/cmake/copy-debug-shaders.cmake"

    COMMENT
        "Copying Forge2D shaders for Debug"
)
