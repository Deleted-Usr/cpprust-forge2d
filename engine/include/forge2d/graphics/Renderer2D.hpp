#pragma once

/// @file Renderer2D.hpp
/// @brief The batched 2D renderer that games draw with.

#include <filesystem>
#include <memory>
#include <string_view>

#include <forge2d/graphics/Colour.hpp>
#include <forge2d/graphics/FrameInfo.hpp>

#include <forge2d/math/Vec2.hpp>
#include <forge2d/math/Transform2D.hpp>

namespace forge
{
    class Texture2D;
    class Camera2D;
    class Font;

    /// @brief Batched 2D renderer for shapes, lines, sprites and text.
    ///
    /// `Application` owns the renderer and passes it to `Game::render()` every frame.
    /// Games normally only call the `draw*()`, `setCamera()` and `resetCamera()` functions.
    ///
    /// ### Coordinates
    ///
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
    ///   shapes share one built-in white texture, and each glyph atlas page used by text
    ///   counts as one texture);
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
    /// texture or font, a non-positive radius or thickness) are ignored, not treated as fatal.
    /// The exception is an invalid text size, which makes `drawText()` throw.
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

            /// @brief Draws a line or block of text.
            ///
            /// Text is laid out left to right, with kerning, and drawn in call order with shapes
            /// and sprites, like any other draw. It isn't saved for a separate pass at the end of
            /// the frame.
            ///
            /// ### **Layout.**
            ///
            /// `\\n`, `\r` and `\r\\n` start a new line at the font's line height. Tabs
            /// jump to the next stop (every four space widths from the start of the line). Other
            /// control characters are skipped. Characters the font doesn't have are drawn as its
            /// "missing glyph" box. This isn't full text shaping: there is no right-to-left
            /// text, ligatures, combining-mark placement, word wrapping, alignment or fallback to
            /// other fonts. Use `Font::measureText()` to align or centre text yourself.
            ///
            /// ### **Appearance.**
            ///
            /// Glyphs are antialiased. There is no rotation parameter, but a rotated
            /// camera turns text along with everything else. Glyphs are rasterised at
            /// @p size pixels per em and stretched with the current coordinates, so a camera zoom
            /// or a high-DPI display magnifies them and they look softer. Positions aren't
            /// snapped to pixels, so whole-number positions give the sharpest text.
            ///
            /// ### **Performance.**
            ///
            /// The first time each glyph is drawn at a given size it is
            /// rasterised and uploaded to a glyph atlas texture; after that it is cached. Each atlas
            /// page uses one of the batch's 8 texture slots.
            ///
            /// @param text     UTF-8 text. An empty string draws nothing. Malformed UTF-8 is drawn
            ///                 as U+FFFD (the replacement character).
            /// @param font     The font to draw with. An empty font (`valid() == false`) draws
            ///                 nothing.
            /// @param position Top-left corner of the first line's box (not the baseline). The
            ///                 first baseline is one ascent below it. Some glyphs can overhang
            ///                 the box slightly.
            /// @param size     Font size in pixels per em, greater than `0` and at most `512`. This
            ///                 is the font's design size, not the height of any letter. Measured in
            ///                 the current coordinates, so it is world units after `setCamera()`.
            /// @param colour   Text colour. Alpha is multiplied with the glyphs' antialiased edges.
            ///                 Defaults to `colours::White`.
            ///
            /// @code
            /// renderer.resetCamera();
            /// renderer.drawText("Score: 1200", font_, { 20, 20 }, 32);
            ///
            /// // Right-align against the window edge.
            /// const auto size = font_.measureText(label, 24);
            /// renderer.drawText(label, font_, { forge::Screen::width() - size.x - 20, 20 }, 24, forge::colours::Yellow);
            /// @endcode
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            /// @throws std::invalid_argument If @p size is zero, negative, non-finite or greater
            ///         than `512`. Unlike other invalid draw arguments, this isn't ignored.
            /// @throws std::length_error If a glyph's atlas space for @p font runs out (16 pages of
            ///         1024×1024 per font, shared by every size), or one glyph is too big for a page.
            ///
            /// @warning Every distinct @p size is rasterised and cached separately, and cached
            ///          glyphs are only freed when the font itself is destroyed. Animating text by
            ///          changing @p size every frame fills the atlas and eventually throws. Use a
            ///          few fixed sizes, or scale with a camera zoom instead.
            void drawText(std::string_view text, const Font& font, math::Vec2 position, float size, Colour colour = colours::White);

            /// @brief Same as the `const Font&` overload, for code that keeps a pointer to a font
            ///        cached by `AssetManager`.
            ///
            /// @param font The font to draw with. `nullptr` or an empty font draws nothing.
            ///
            /// @code
            /// const forge::Font* font_ = &assets.loadFont("fonts/MyFont.ttf");   // in onStart()
            /// renderer.drawText("Paused", font_, { 20, 20 }, 32);                  // in render()
            /// @endcode
            ///
            /// @pre Called between `beginFrame()` and `endFrame()`.
            /// @throws std::invalid_argument As for the `const Font&` overload.
            /// @throws std::length_error     As for the `const Font&` overload.
            void drawText(std::string_view text, const Font* font, math::Vec2 position, float size, Colour colour = colours::White);

        private:
            struct Impl;
            std::unique_ptr<Impl> impl_;

    };
}
