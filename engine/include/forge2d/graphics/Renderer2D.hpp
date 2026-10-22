#pragma once

#include <forge2d/graphics/Colour.hpp>

namespace forge
{
    class Renderer2D
    {
        public:
            Renderer2D() = default;

            void beginFrame(const Colour& clearColour);
            void endFrame();
    };
}
