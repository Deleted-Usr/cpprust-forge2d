#pragma once

#include <filesystem>
#include <memory>

#include <forge2d/graphics/Colour.hpp>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Transform2D.hpp>

namespace forge
{
    class Texture2D;
    class Camera2D;

    class Renderer2D
    {
        public:
            explicit Renderer2D(const std::filesystem::path& shaderDirectory);

            ~Renderer2D();

            Renderer2D(const Renderer2D&) = delete;
            Renderer2D& operator=(const Renderer2D&) = delete;

            void beginFrame(float width, float height, const Colour& clearColour);
            void endFrame();

            void setCamera(const Camera2D& cam);

            void drawTriangle (math::Transform2D transform,     Colour colour, bool fill = true);
            void drawRect     (math::Transform2D transform,     Colour colour, bool fill = true);
            void drawCircle   (math::Vec2 centre, float radius, Colour colour, bool fill = true);
            void drawLine     (math::Vec2 from, math::Vec2 to,  Colour colour, float thickness = 1.0f);

            void drawSprite(const Texture2D& texture, math::Transform2D transform, Colour tint = colours::White);

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;

            void initTriangle();
            void initQuad();
            void initLine();
    };
}
