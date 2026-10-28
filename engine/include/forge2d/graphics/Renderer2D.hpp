#pragma once

#include <filesystem>
#include <memory>

#include <forge2d/graphics/Colour.hpp>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Transform2D.hpp>

namespace forge
{
    class Renderer2D
    {
        public:
            explicit Renderer2D(const std::filesystem::path& shaderDirectory);

            ~Renderer2D();

            Renderer2D(const Renderer2D&) = delete;
            Renderer2D& operator=(const Renderer2D&) = delete;

            void beginFrame(const Colour& clearColour);
            void endFrame();

            void drawTriangle(math::Vec2 a, math::Vec2 b, math::Vec2 c, Colour colour) const;
            void drawRect(math::Transform2D transform, Colour colour) const;
            void drawLine(math::Vec2 from, math::Vec2 to, Colour colour);
            void drawCircle(math::Vec2 centre, float radius, Colour colour);

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;

            void initTriangle();
            void initQuad();
    };
}
