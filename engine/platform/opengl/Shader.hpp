#pragma once

#include <filesystem>
#include <string>

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
