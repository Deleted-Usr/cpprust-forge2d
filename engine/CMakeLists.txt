add_library(
    forge2d
    STATIC

    # --------------------------------------------------------
    # Core
    # --------------------------------------------------------

    src/core/Application.cpp
    src/core/Window.cpp

    # --------------------------------------------------------
    # GLFW platform
    # --------------------------------------------------------

    # platform/glfw/GlfwWindow.cpp
    # platform/glfw/GlfwInput.cpp

    # --------------------------------------------------------
    # OpenGL
    # --------------------------------------------------------

    platform/opengl/Renderer2D.cpp
    platform/opengl/OpenGLContext.cpp
    # platform/opengl/GLShader.cpp
    # platform/opengl/GLTexture.cpp
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

target_link_libraries(
    forge2d

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
