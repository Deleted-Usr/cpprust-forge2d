#pragma once

namespace forge::internal::opengl
{
    /// Routes OpenGL's own error/warning messages into the engine logger.
    /// Requires a debug context (GLFW_OPENGL_DEBUG_CONTEXT); does nothing otherwise.
    void enableDebugOutput();

    /// Logs vendor / renderer / GL version once at startup.
    void logDriverInfo();
}
