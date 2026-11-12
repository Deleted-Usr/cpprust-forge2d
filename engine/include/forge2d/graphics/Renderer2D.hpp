#pragma once

/// @file Renderer2D.hpp
/// @brief The batched 2D renderer that games draw with.

#include <filesystem>
#include <memory>

#include <forge2d/graphics/Colour.hpp>
#include <forge2d/graphics/FrameInfo.hpp>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Transform2D.hpp>

namespace forge
{
    class Texture2D;
    class Camera2D;

    /// @brief Batched 2D renderer for shapes, lines and sprites.
    ///
    /// `Application` owns the renderer and passes it to `Game::render()` every frame.
    /// Games normally only call the `draw*()`, `setCamera()` and `resetCamera()` functions.
    ///
    /// ### Coordinates
    /// Each frame starts in **screen space**: the origin is the top-left corner of the window,
    /// +X points right, +Y points down, and one unit is one logical window coordinate
    /// (see `FrameInfo::logicalSize`). After `setCamera()`, coordinates are **world units**
    /// seen through that camera until `resetCamera()` or the end of the frame.
    ///
    /// Rotations are in radians. Because +Y points down, a positive rotation turns shapes
    /// **clockwise** on screen.
    ///
    /// ### Batching
    /// Draw calls are collected into batches and sent to the GPU together, so drawing many
    /// small shapes is cheap. A batch is flushed (sent to the GPU) when:
    /// - it runs out of vertex or index space;
    /// - a draw needs a 9th texture (a batch holds up to 8 textures; all untextured
    ///   shapes share one built-in white texture);
    /// - `setCamera()` or `resetCamera()` is called;
    /// - `endFrame()` is called.
    ///
    /// Shapes are drawn in call order, later on top of earlier. There is no depth sorting.
    /// To keep the number of flushes low, group sprite draws by texture or use a texture atlas.
    ///
    /// ### Blending
    /// Alpha blending is always on. Colours use straight (non-premultiplied) alpha.
    ///
    /// ### Threading
    /// All functions must be called on the main (GL) thread.
    ///
    /// ### Errors
    /// Calling a draw function outside `beginFrame()`/`endFrame()` is a programming error:
    /// Debug builds assert, and other builds ignore the call. Invalid draw arguments (an empty
    /// texture, a non-positive radius or thickness) are ignored, not treated as fatal.
    ///
    /// @code
    /// void render(forge::Renderer2D& renderer) override
    /// {
    ///     renderer.setCamera(camera_);   // world space
    ///     renderer.drawSprite(playerTexture_, { .position = player_.position(), .scale = { 32, 32 } });
    ///
    ///     renderer.resetCamera();        // back to screen space for the HUD
    ///     renderer.drawRect({ .position = { 16, 16 }, .scale = { 200, 20 } }, forge::colours::Red);
    /// }
    /// @endcode
    class Renderer2D
    {
        public:
            /// @brief Creates the renderer and its GPU resources.
            ///
            /// @param shaderDirectory Folder containing `batch.vert.glsl` and `batch.frag.glsl`.
            ///        Only Debug builds read it, so shaders can be edited without recompiling.
            ///        All other build types use shaders embedded in the binary and ignore it.
            ///
            /// @pre An OpenGL 4.6 context is current on this thread (`Window` creates one).
            /// @throws std::runtime_error If a shader file can't be opened, or a shader fails
            ///         to compile or link.
            explicit Renderer2D(const std::filesystem::path& shaderDirectory);

            /// @brief Releases the renderer's GPU resources.
            /// @pre The GL context that created the renderer still exists.
            ~Renderer2D();

            Renderer2D(const Renderer2D&) = delete;
            Renderer2D& operator=(const Renderer2D&) = delete;

            /// @brief Starts a frame: clears the screen and resets to screen-space coordinates.
            ///
            /// Called by `Application` before `Game::render()`. Games don't need to call it.
            ///
            /// If the frame has no drawable area (for example when the window is minimised and
            /// any size in @p frame is zero), the whole frame is skipped: nothing is cleared
            /// and every draw call until `endFrame()` is ignored.
            ///
            /// @param frame       Window sizes for this frame.
            /// @param clearColour Colour the framebuffer is cleared to.
            ///
            /// @pre The previous frame has ended (Debug builds assert).
            void beginFrame(const FrameInfo& frame, const Colour& clearColour);

            /// @brief Flushes any remaining batched draws and ends the frame.
            ///
            /// Called by `Application` after `Game::render()`. Games don't need to call it.
            ///
            /// @pre `beginFrame()` was called (Debug builds assert; other builds ignore the call).
            void endFrame();

            /// @brief Draws everything after this call through @p cam.
            ///
            /// Flushes draws made so far, then switches to world-space coordinates. The camera's
            /// state is captured **now**: changing the camera afterwards doesn't affect this frame
            /// unless `setCamera()` is called again.
            ///
            /// @param cam The camera to view through. Call `Camera2D::update()` before this,
            ///            during `Game::update()`.
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            void setCamera(const Camera2D& cam);

            /// @brief Returns to screen-space coordinates, for HUD and UI drawing.
            ///
            /// Flushes draws made so far through the previous camera.
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            void resetCamera();

            /// @brief Draws an upward-pointing isosceles triangle.
            ///
            /// The triangle fills the box described by @p transform: its base runs along the
            /// bottom edge and its tip is at the middle of the top edge.
            ///
            /// @param transform `position` is the box's top-left corner, `scale` is its width and
            ///                  height, and `rotation` turns it around that top-left corner.
            /// @param colour    Fill or outline colour.
            /// @param fill      `true` for a solid triangle, `false` for an outline 1 unit wide,
            ///                  centred on the edges.
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            void drawTriangle (math::Transform2D transform,     Colour colour, bool fill = true);

            /// @brief Draws a rectangle.
            ///
            /// @param transform `position` is the top-left corner, `scale` is the width and
            ///                  height, and `rotation` turns the rectangle around its
            ///                  **top-left corner** (not its centre).
            /// @param colour    Fill or outline colour.
            /// @param fill      `true` for a solid rectangle, `false` for an outline 1 unit wide,
            ///                  centred on the edges.
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            void drawRect     (math::Transform2D transform,     Colour colour, bool fill = true);

            /// @brief Draws a circle, approximated with 64 segments.
            ///
            /// @param centre Centre of the circle.
            /// @param radius Radius. Zero, negative or non-finite values draw nothing.
            /// @param colour Fill or outline colour.
            /// @param fill   `true` for a solid disc, `false` for an outline 1 unit wide,
            ///               centred on the circle's edge.
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            void drawCircle   (math::Vec2 centre, float radius, Colour colour, bool fill = true);

            /// @brief Draws a straight line with flat ends.
            ///
            /// @param from      Start point.
            /// @param to        End point. If it equals @p from, nothing is drawn.
            /// @param colour    Line colour.
            /// @param thickness Width of the line, centred on the line between the points.
            ///                  Zero or negative values draw nothing.
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            void drawLine     (math::Vec2 from, math::Vec2 to,  Colour colour, float thickness = 1.0f);

            /// @brief Draws a texture stretched over a rectangle.
            ///
            /// The whole texture is mapped onto the rectangle, upright, with its top-left texel
            /// at the rectangle's top-left corner.
            ///
            /// @param texture   The texture to draw. An empty texture (`valid() == false`) draws
            ///                  nothing and logs an error once per call site.
            /// @param transform `position` is the top-left corner, `scale` is the drawn width and
            ///                  height (not a multiplier of the texture's size), and `rotation`
            ///                  turns the sprite around its **top-left corner**.
            /// @param tint      Multiplied with each texel. `colours::White` leaves the texture
            ///                  unchanged, and lowering `a` makes it translucent.
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            ///
            /// @note To draw a texture at its own size, use its dimensions as the scale:
            ///       `{ .position = pos, .scale = { float(tex.width()), float(tex.height()) } }`.
            void drawSprite(const Texture2D& texture, math::Transform2D transform, Colour tint = colours::White);

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;

    };
}