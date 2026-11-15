#include <forge2d/core/Screen.hpp>

#include <forge2d/debug/Assert.hpp>
#include <forge2d/graphics/FrameInfo.hpp>

namespace
{
    forge::FrameInfo g_frame{};
    bool             g_attached = false;

    const forge::FrameInfo& frame() noexcept
    {
        FORGE_ASSERT(g_attached, "forge::Screen used with no running Application");
        return g_frame;
    }
}

namespace forge
{
    float      Screen::width()  noexcept { return frame().logicalSize.x; }
    float      Screen::height() noexcept { return frame().logicalSize.y; }
    math::Vec2 Screen::size()   noexcept { return frame().logicalSize; }
    math::Vec2 Screen::centre() noexcept { return frame().logicalSize * 0.5f; }

    bool Screen::isMinimised() noexcept
    {
        const auto& f = frame();
        return f.framebufferWidth == 0 || f.framebufferHeight == 0;
    }

    void Screen::attach(const FrameInfo& initial) noexcept { g_frame = initial; g_attached = true; }
    void Screen::update(const FrameInfo& frame)   noexcept { g_frame = frame; }
    void Screen::detach()                         noexcept { g_frame = {}; g_attached = false; }
}
