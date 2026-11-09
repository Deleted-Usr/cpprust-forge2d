add_library(
    forge2d
    STATIC

    # --------------------------------------------------------
    # Core
    # --------------------------------------------------------

    src/assets/AssetManager.cpp

    src/core/Application.cpp
    src/core/Window.cpp

    src/input/InputManager.cpp

    # --------------------------------------------------------
    # Math / Time
    # --------------------------------------------------------

    src/math/Mat4.cpp
    src/math/Transform2D.cpp

    src/time/Time.cpp
    src/time/Clock.cpp

    # --------------------------------------------------------
    # Graphics
    # --------------------------------------------------------

    src/graphics/Renderer2D.cpp
    src/graphics/Texture2D.cpp
    src/graphics/Camera2D.cpp
    src/graphics/internal/BatchBuilder2D.cpp

    # --------------------------------------------------------
    # Images
    # --------------------------------------------------------

    platform/stb/stb_image.cpp
    src/assets/Image.cpp

    # --------------------------------------------------------
    # Debug
    # --------------------------------------------------------

    src/debug/Log.cpp
    src/debug/Assert.cpp

    # --------------------------------------------------------
    # GLFW
    # --------------------------------------------------------



    # --------------------------------------------------------
    # OpenGL
    # --------------------------------------------------------

    platform/opengl/OpenGLContext.cpp
    platform/opengl/OpenGLRenderer2D.cpp
    platform/opengl/OpenGLDebug.cpp

    platform/opengl/shaders/Shader.cpp
    platform/opengl/textures/GPUTexture.cpp

    platform/opengl/buffer/Buffer.cpp
    platform/opengl/array/VertexArray.cpp
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

    PUBLIC
        $<$<CONFIG:Debug>:FORGE2D_DEBUG>
)
