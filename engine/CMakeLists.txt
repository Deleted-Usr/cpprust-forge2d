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
    src/graphics/Font.cpp
    src/graphics/internal/GlyphAtlas.cpp
    src/graphics/internal/TextRenderer.cpp
    src/graphics/internal/BatchBuilder2D.cpp

    # --------------------------------------------------------
    # Images
    # --------------------------------------------------------

    src/assets/Image.cpp

    # --------------------------------------------------------
    # Debug
    # --------------------------------------------------------

    src/debug/Log.cpp
    src/debug/Assert.cpp

    # --------------------------------------------------------
    # GLFW
    # --------------------------------------------------------

    platform/glfw/input/GLFWInput.cpp

    platform/glfw/input/KeyTranslation.cpp
    platform/glfw/input/MouseButtonTranslation.cpp

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
    src/core/Screen.cpp
)

# Namespaced alias, so consumers can write `target_link_libraries(game PRIVATE forge2d::forge2d)`.
add_library(forge2d::forge2d ALIAS forge2d)

# ============================================================
# Embedded Shaders
# ============================================================

# Keep the GLSL files as the single source of truth. Editing either file
# triggers reconfiguration and regeneration on the next build.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/batch.vert.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/batch.frag.glsl"
)

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/shaders/batch.vert.glsl" FORGE2D_BATCH_VERTEX)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/shaders/batch.frag.glsl" FORGE2D_BATCH_FRAGMENT)

configure_file(
    platform/opengl/shaders/BatchShaders.hpp.in
    "${CMAKE_CURRENT_BINARY_DIR}/generated/BatchShaders.hpp"
    @ONLY
)
target_include_directories(forge2d PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_compile_definitions(forge2d PRIVATE $<$<NOT:$<CONFIG:Debug>>:FORGE2D_EMBED_BATCH_SHADERS>)

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
        forge2d::rust
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

option(FORGE2D_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(FORGE2D_WARNINGS_AS_ERRORS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(forge2d PRIVATE -Werror)
    elseif(MSVC)
        target_compile_options(forge2d PRIVATE /WX)
    endif()
endif()
