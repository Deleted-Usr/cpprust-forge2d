#include <opengl/OpenGLDebug.hpp>

#include <string_view>

#include <glad/gl.h>

#include <forge2d/debug/Log.hpp>
#include <opengl/GLTypeConversions.hpp>

namespace forge::internal::opengl
{
    void enableDebugOutput()
    {
        GLint flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

        if ((flags & GL_CONTEXT_FLAG_DEBUG_BIT) == 0)
        {
            FORGE_LOG_WARN("OpenGL debug output unavailable (not a debug context)");
            return;
        }

        glEnable(GL_DEBUG_OUTPUT);

        // Synchronous: the callback runs inside the offending GL call, on this thread.
        // Slower, but the stack trace is meaningful. Debug builds only.
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        glDebugMessageCallback(&onDebugMessage, nullptr);

        // Drop "notification" spam (e.g. NVIDIA's "buffer will use VIDEO memory").
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);

        FORGE_LOG_DEBUG("OpenGL debug output enabled");
    }

    void logDriverInfo()
    {
        const auto str = [](const GLenum name) {
            const auto* s = reinterpret_cast<const char*>(glGetString(name));
            return std::string_view(s ? s : "?");
        };

        FORGE_LOG_INFO("OpenGL {} | {} | {}", str(GL_VERSION), str(GL_RENDERER), str(GL_VENDOR));
    }
}
