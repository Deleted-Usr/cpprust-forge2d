#pragma once

#include <filesystem>
#include <memory>

#include <forge2d/graphics/Colour.hpp>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Transform2D.hpp>


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

            void drawTriangle(math::Vec2 a, math::Vec2 b, math::Vec2 c, Colour colour);
            void drawRect(math::Transform2D transform, Colour colour);
            void drawLine(math::Vec2 from, math::Vec2 to, Colour colour);
            void drawCircle(math::Vec2 centre, float radius, Colour colour);

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
