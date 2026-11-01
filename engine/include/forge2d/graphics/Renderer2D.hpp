#pragma once

#include <filesystem>
#include <memory>

#include <forge2d/graphics/Colour.hpp>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Transform2D.hpp>

namespace forge
{
    class Texture2D;

    class Renderer2D
    {
        public:
            explicit Renderer2D(const std::filesystem::path& shaderDirectory);

            ~Renderer2D();

            Renderer2D(const Renderer2D&) = delete;
            Renderer2D& operator=(const Renderer2D&) = delete;

            void beginFrame(const Colour& clearColour);
            void endFrame();

            void drawTriangle (math::Transform2D transform,     Colour colour, bool fill = true) const;
            void drawRect     (math::Transform2D transform,     Colour colour, bool fill = true) const;
            void drawCircle   (math::Vec2 centre, float radius, Colour colour, bool fill = true) const;
            void drawLine     (math::Vec2 from, math::Vec2 to,  Colour colour)                   const;

            void drawSprite(const Texture2D& texture, math::Transform2D transform, Colour tint = colours::White) const;

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;

            void initTriangle();
            void initQuad();
    };
}
