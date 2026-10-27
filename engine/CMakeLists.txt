add_library(
    forge2d
    STATIC

    # --------------------------------------------------------
    # Core
    # --------------------------------------------------------

    src/core/Application.cpp
    src/core/Window.cpp

    # --------------------------------------------------------
    # Math
    # --------------------------------------------------------

    src/math/Mat4.cpp
    src/math/Transform2D.cpp
    src/time/Time.cpp
    src/time/Clock.cpp

    # --------------------------------------------------------
    # GLFW platform
    # --------------------------------------------------------

    # platform/glfw/GlfwWindow.cpp
    # platform/glfw/GlfwInput.cpp

    # --------------------------------------------------------
    # OpenGL
    # --------------------------------------------------------

    platform/opengl/Renderer2D.cpp
    platform/opengl/Shader.cpp
    platform/opengl/OpenGLContext.cpp
)

# ============================================================
# Public headers
# ============================================================

target_include_directories(
    forge2d

    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"

    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/platform"
)

# ============================================================
# Language version
# ============================================================

target_compile_features(
    forge2d
    PUBLIC
        cxx_std_20
)

# ============================================================
# Dependencies
# ============================================================

include(FetchContent)

# GLM is used as a header-only library.
set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS   OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glm

    GIT_REPOSITORY
        https://github.com/g-truc/glm.git

    GIT_TAG
        1.0.3

    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glm)

target_link_libraries(
    forge2d

    PUBLIC
        glm::glm-header-only

    PRIVATE
        glfw
        glad::glad
)

# ============================================================
# Compiler warnings
# ============================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
       forge2d

       PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
elseif(MSVC)
    target_compile_options(
       forge2d

       PRIVATE
            /W4
    )
endif()

# ============================================================
# Debug definitions
# ============================================================

target_compile_definitions(
    forge2d

    PRIVATE
        $<$<CONFIG:Debug>:FORGE2D_DEBUG>
)
