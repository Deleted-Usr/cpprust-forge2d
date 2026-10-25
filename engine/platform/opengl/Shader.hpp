#pragma once

#include <filesystem>
#include <string>

#include <forge2d/graphics/Colour.hpp>

namespace forge
{
    class Shader
    {
        public:
            Shader(
                const std::filesystem::path& vertexPath,
                const std::filesystem::path& fragmentPath
            );

            ~Shader();

            Shader(const Shader&) = delete;
            Shader& operator=(const Shader&) = delete;

            void bind() const;

            void setBool(const std::string& name, bool value) const;
            void setInt(const std::string& name, int value) const;
            void setFloat(const std::string& name, float value) const;

            void setColour(const std::string& name, const Colour& value) const;

        private:
            unsigned int program_ = 0;

            static std::string readFile(const std::filesystem::path& path);

            static unsigned int compile(
                unsigned int type,
                const std::string& source,
                const std::filesystem::path& path
            );
    };
}
