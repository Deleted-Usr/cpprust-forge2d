#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>

#include <forge2d/graphics/Colour.hpp>

#include <forge2d/math/Vec2.hpp>
#include "forge2d/math/Mat4.hpp"

namespace forge::internal::opengl
{
    class Shader
    {
        public:
            struct Source
            {
                std::string_view code;
                std::string_view name;
            };

            Shader(Source vertex, Source fragment);

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
            mutable std::unordered_map<std::string, int> uniformLocations_;
            int uniformLocation(std::string_view name) const;

            static std::string readFile(const std::filesystem::path& path);

            static unsigned int compile(
                unsigned int type,
                Source source
            );
    };
}
