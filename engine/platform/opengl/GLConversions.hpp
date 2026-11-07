#pragma once

#include <string_view>
#include <glad/gl.h>

#include<opengl/buffer/Buffer.hpp>

#include <forge2d/debug/Log.hpp>

namespace forge::internal::opengl
{
    inline GLenum openGLBufferType(const BufferType type)
    {
        switch (type)
        {
            case BufferType::ARRAY_BUFFER:   return GL_ARRAY_BUFFER;
            case BufferType::ELEMENT_BUFFER: return GL_ELEMENT_ARRAY_BUFFER;
        }

        return GL_NONE;
    }

    inline GLenum openGLBufferUsage(const BufferUsage usage)
    {
        switch (usage)
        {
            case BufferUsage::STATIC:  return GL_STATIC_DRAW;
            case BufferUsage::DYNAMIC: return GL_DYNAMIC_DRAW;
            case BufferUsage::STREAM:  return GL_STREAM_DRAW;
        }

        return GL_NONE;
    }

    inline std::string_view sourceName(const GLenum source)
    {
        switch (source)
        {
            case GL_DEBUG_SOURCE_API:             return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "Window System";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
            case GL_DEBUG_SOURCE_THIRD_PARTY:     return "Third Party";
            case GL_DEBUG_SOURCE_APPLICATION:     return "Application";

            default:                              return "Other";
        }
    }

    inline std::string_view typeName(const GLenum type)
    {
        switch (type)
        {
            case GL_DEBUG_TYPE_ERROR:               return "Error";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "Undefined Behaviour";
            case GL_DEBUG_TYPE_PORTABILITY:         return "Portability";
            case GL_DEBUG_TYPE_PERFORMANCE:         return "Performance";
            case GL_DEBUG_TYPE_MARKER:              return "Marker";

            default:                                return "Other";
        }
    }

    inline log::Level toLogLevel(const GLenum type, const GLenum severity)
    {
        if (type == GL_DEBUG_TYPE_ERROR)
        {
            return log::Level::ERROR;
        }

        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:   return log::Level::ERROR;
            case GL_DEBUG_SEVERITY_MEDIUM: return log::Level::WARN;
            case GL_DEBUG_SEVERITY_LOW:    return log::Level::INFO;

            default:                       return log::Level::TRACE;
        }
    }

    inline void GLAD_API_PTR onDebugMessage(
        const GLenum source,
        const GLenum type,
        const GLuint id,
        const GLenum severity,
        const GLsizei length,
        const GLchar* message,
        const void* /*userParam*/)
    {
        const log::Level level = toLogLevel(type, severity);

        if (!log::enabled(level))
        {
            return;
        }

        // The location points here, not at the GL call that caused it. Because
        // output is SYNCHRONOUS, a breakpoint on this line shows the real culprit
        // in the call stack.
        FORGE_LOG(level, "[OpenGL] {} / {} (id {}): {}", sourceName(source), typeName(type), id, std::string_view(message, static_cast<std::size_t>(length)));
    }
}
