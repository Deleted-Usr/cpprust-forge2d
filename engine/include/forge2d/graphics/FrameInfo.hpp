#pragma once

/// @file FrameInfo.hpp
/// @brief Window sizes the renderer needs for each frame.

#include <forge2d/math/Vec2.hpp>

namespace forge
{
    /// @brief The sizes of the area being drawn this frame.
    ///
    /// Engine-facing: @p Window::frameInfo() produces it and @p Renderer2D::beginFrame() reads it.
    /// Games normally don't need it.
    ///
    /// Logical size and framebuffer size can differ. On high-DPI displays, a window that is
    /// 800×600 logical units may have a 1600×1200 pixel framebuffer. Game coordinates always use
    /// the logical size, so a game looks the same on any display.
    ///
    /// If any size is zero (for example while the window is minimised), the renderer skips the
    /// frame.
    struct FrameInfo
    {
        /// Size of the window in logical (screen-space) units. This is the coordinate space draw
        /// calls use when no camera is set.
        math::Vec2 logicalSize{};

        /// Width of the framebuffer in physical pixels.
        int framebufferWidth = 0;

        /// Height of the framebuffer in physical pixels.
        int framebufferHeight = 0;
    };
}