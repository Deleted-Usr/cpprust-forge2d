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
            void drawQuad();

        private:
            std::unique_ptr<Shader> shapeShader_;

            unsigned int triangleVAO_ = 0;
            unsigned int triangleVBO_ = 0;

            unsigned int quadVAO_ = 0;
            unsigned int quadVBO_ = 0;
            unsigned int quadEBO_ = 0;

            void initTriangle();
            void initQuad();
    };
}
