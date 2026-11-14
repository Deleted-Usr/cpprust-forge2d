# ------------------------------------------------------------
# Helpers for games built on Forge2D
# ------------------------------------------------------------

# Captured here so the helpers work when Forge2D is added with FetchContent or
# add_subdirectory, where PROJECT_SOURCE_DIR would be the consumer's project.
set(FORGE2D_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/.." CACHE INTERNAL "Forge2D source directory")

# forge2d_copy_shaders(<target>)
#
# Debug builds load the batch shaders from a `shaders/` folder next to the
# executable. This copies them there after each Debug build of <target>.
# Other build types embed the shaders, so nothing is copied.
function(forge2d_copy_shaders target)
    add_custom_command(
        TARGET ${target}
        POST_BUILD

        COMMAND
            ${CMAKE_COMMAND}
            "-DCONFIG=$<CONFIG>"
            "-DSOURCE=${FORGE2D_SOURCE_DIR}/engine/shaders"
            "-DDESTINATION=$<TARGET_FILE_DIR:${target}>/shaders"
            -P "${FORGE2D_SOURCE_DIR}/cmake/copy-debug-shaders.cmake"

        COMMENT
            "Copying Forge2D shaders for Debug"
    )
endfunction()
