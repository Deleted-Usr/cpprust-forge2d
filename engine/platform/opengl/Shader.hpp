#pragma once

#include <filesystem>
#include <string>

#include <forge2d/graphics/Colour.hpp>

#include <forge2d/math/Vec2.hpp>
#include "forge2d/math/Mat4.hpp"

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

            void setBool(const std::string_view& name, bool value) const;
            void setInt(const std::string_view& name, int value) const;
            void setFloat(const std::string_view& name, float value) const;
            void setVec4(const std::string_view& name, const math::Vec4& value) const;
            void setMat4(const std::string_view& name, const math::Mat4& value) const;

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
