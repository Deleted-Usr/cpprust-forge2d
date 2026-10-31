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

            void drawTriangle (math::Transform2D transform, Colour colour)     const;
            void drawRect     (math::Transform2D transform, Colour colour)     const;
            void drawLine     (math::Vec2 from, math::Vec2 to,  Colour colour) const;
            void drawCircle   (math::Vec2 centre, float radius, Colour colour) const;
            void drawSprite   (math::Transform2D transform)                    const;

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;

            void initTriangle();
            void initQuad();

            void loadTexture(unsigned int& tex, const std::string& path, int format);
    };
}
