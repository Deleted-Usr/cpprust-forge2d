#include <forge2d/graphics/Renderer2D.hpp>

#include <glad/gl.h>

namespace forge
{
    void Renderer2D::beginFrame(const Colour& clearColor)
    {
        glClearColor(
            clearColor.r,
            clearColor.g,
            clearColor.b,
            clearColor.a
        );

        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer2D::endFrame()
    {
        // Nothing yet.
    }
}
