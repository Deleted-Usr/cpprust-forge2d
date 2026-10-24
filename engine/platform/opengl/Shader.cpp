#include "Shader.hpp"

#include <glad/gl.h>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace forge
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

    unsigned int Shader::compile(unsigned int type, const std::string& source, const std::filesystem::path& path)
    {
        const unsigned int shader = glCreateShader(type);
        const char* sourcePointer = source.c_str();

        glShaderSource(shader,1,&sourcePointer,nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader,GL_COMPILE_STATUS,&success);

        if (!success)
        {
            int logLength = 0;
            glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&logLength);

            std::vector<char> log(static_cast<std::size_t>(logLength));
            glGetShaderInfoLog(shader,logLength,nullptr,log.data());

            glDeleteShader(shader);

            throw std::runtime_error(
                "Failed to compile shader "
                + path.string()
                + ":\n"
                + std::string(log.data())
            );
        }

        return shader;
    }

    Shader::Shader(
        const std::filesystem::path& vertexPath,
        const std::filesystem::path& fragmentPath
    )
    {
        const std::string vertexSource   = readFile(vertexPath);
        const std::string fragmentSource = readFile(fragmentPath);

        unsigned int vertexShader   = 0;
        unsigned int fragmentShader = 0;

        try
        {
            vertexShader   = compile(GL_VERTEX_SHADER,   vertexSource,   vertexPath);
            fragmentShader = compile(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);

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
                std::vector<char> log(static_cast<std::size_t>(logLength));

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
}
