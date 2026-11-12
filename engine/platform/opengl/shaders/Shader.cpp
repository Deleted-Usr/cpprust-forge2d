#include "Shader.hpp"

#include <glad/gl.h>

#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace forge::internal::opengl
{
    std::string Shader::readFile(const std::filesystem::path& path)
    {
        std::ifstream file(path, std::ios::in | std::ios::binary);

        if (!file)
        {
            throw std::runtime_error("Failed to open shader: " + path.string());
        }

        std::ostringstream stream;
        stream << file.rdbuf();

        return stream.str();
    }

    unsigned int Shader::compile(unsigned int type, Source source)
    {
        const unsigned int shader = glCreateShader(type);
        const char* sourcePointer = source.code.data();
        const auto sourceLength = static_cast<GLint>(source.code.size());

        glShaderSource(shader, 1, &sourcePointer, &sourceLength);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader,GL_COMPILE_STATUS,&success);

        if (!success)
        {
            int logLength = 0;
            glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&logLength);

            std::vector<char> log(static_cast<std::size_t>(std::max(logLength, 1)), '\0');
            glGetShaderInfoLog(shader,logLength,nullptr,log.data());

            glDeleteShader(shader);

            throw std::runtime_error(
                "Failed to compile shader "
                + std::string(source.name)
                + ":\n"
                + std::string(log.data())
            );
        }

        return shader;
    }

    Shader::Shader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
    : Shader(
        Source{ readFile(vertexPath), vertexPath.string() },
        Source{ readFile(fragmentPath), fragmentPath.string() }
    ) { }

    Shader::Shader(Source vertex, Source fragment)
    {
        unsigned int vertexShader   = 0;
        unsigned int fragmentShader = 0;

        try
        {
            vertexShader   = compile(GL_VERTEX_SHADER,   vertex);
            fragmentShader = compile(GL_FRAGMENT_SHADER, fragment);

            program_ = glCreateProgram();

            glAttachShader(program_, vertexShader);
            glAttachShader(program_, fragmentShader);

            glLinkProgram(program_);

            int success = 0;
            glGetProgramiv(program_, GL_LINK_STATUS, &success);

            if (!success)
            {
                int logLength = 0;

                glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &logLength);
                std::vector<char> log(static_cast<std::size_t>(std::max(logLength, 1)), '\0');

                glGetProgramInfoLog(program_, logLength, nullptr, log.data());

                throw std::runtime_error(
                    "Failed to link shader program:\n"
                    + std::string(log.data())
                );
            }
        }
        catch (...)
        {
            if (vertexShader != 0)
            {
                glDeleteShader(vertexShader);
            }

            if (fragmentShader != 0)
            {
                glDeleteShader(fragmentShader);
            }

            if (program_ != 0)
            {
                glDeleteProgram(program_);
                program_ = 0;
            }

            throw;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    Shader::~Shader()
    {
        if (program_ != 0)
        {
            glDeleteProgram(program_);
        }
    }

    void Shader::bind() const
    {
        glUseProgram(program_);
    }

    int Shader::uniformLocation(std::string_view name) const
    {
        const std::string key(name);

        if (const auto found = uniformLocations_.find(key); found != uniformLocations_.end())
        {
            return found->second;
        }

        const int location = glGetUniformLocation(program_, key.c_str());
        uniformLocations_.emplace(key, location);

        return location;
    }

    void Shader::setBool(const std::string_view& name, const bool value) const
    {
        glUniform1i(uniformLocation(name), static_cast<int>(value));
    }

    void Shader::setInt(const std::string_view& name, const int value) const
    {
        glUniform1i(uniformLocation(name), value);
    }

    void Shader::setFloat(const std::string_view& name, const float value) const
    {
        glUniform1f(uniformLocation(name), value);
    }

    void Shader::setVec4(const std::string_view& name, const math::Vec4& value) const
    {
        glUniform4f(uniformLocation(name), value.x, value.y, value.z, value.w);
    }

    void Shader::setMat4(const std::string_view& name, const math::Mat4& value) const
    {
        glUniformMatrix4fv(uniformLocation(name), 1, GL_FALSE, value.data());
    }
}
