include(FetchContent)

# ============================================================
# GLFW
# ============================================================

# Only want the GLFW library itself.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw

    GIT_REPOSITORY
        https://github.com/glfw/glfw.git

    GIT_TAG
        3.5.1
)

FetchContent_MakeAvailable(glfw)

# ============================================================
# GLAD
# ============================================================

add_library(
    glad
    STATIC

    "${PROJECT_SOURCE_DIR}/external/glad/src/gl.c"
)

target_include_directories(
    glad

    PUBLIC
        "${PROJECT_SOURCE_DIR}/external/glad/include"
)

# Namespaced alias.
#
# Both of these will work:
#
#   glad
#   glad::glad
#
add_library(
    glad::glad
    ALIAS glad
)
