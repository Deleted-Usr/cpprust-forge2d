#pragma once

#include <filesystem>
#include <memory>

#include <forge2d/graphics/Colour.hpp>

namespace forge
{
    class Shader;

    class Renderer2D
    {
        public:
            explicit Renderer2D(const std::filesystem::path& shaderDirectory);

            ~Renderer2D();

            Renderer2D(const Renderer2D&) = delete;
            Renderer2D& operator=(const Renderer2D&) = delete;

            void beginFrame(const Colour& clearColour);
            void endFrame();

            void drawTriangle();

        private:
            std::unique_ptr<Shader> triangleShader_;

            unsigned int triangleVao_ = 0;
            unsigned int triangleVbo_ = 0;
    };
}
